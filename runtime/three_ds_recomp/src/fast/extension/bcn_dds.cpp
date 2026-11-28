#include "bcn_dds.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>

#define BCDEC_IMPLEMENTATION
#define BCDECDEF static inline
#include "third_party/bcdec.h"

namespace Oot3d::Renderer {
namespace {

enum class Bcn { BC1, BC2, BC3, BC4, BC5, BC7 };

constexpr uint32_t FourCc(char a, char b, char c, char d) {
    return uint32_t(a) | (uint32_t(b) << 8) | (uint32_t(c) << 16) | (uint32_t(d) << 24);
}

uint32_t Read32(std::span<const uint8_t> input, size_t offset) {
    return uint32_t(input[offset]) | (uint32_t(input[offset + 1]) << 8) |
           (uint32_t(input[offset + 2]) << 16) | (uint32_t(input[offset + 3]) << 24);
}

Bcn ParseFormat(uint32_t fourCc, uint32_t dxgi) {
    switch (fourCc) {
        case FourCc('D','X','T','1'): return Bcn::BC1;
        case FourCc('D','X','T','3'): return Bcn::BC2;
        case FourCc('D','X','T','5'): return Bcn::BC3;
        case FourCc('A','T','I','1'):
        case FourCc('B','C','4','U'): return Bcn::BC4;
        case FourCc('A','T','I','2'):
        case FourCc('B','C','5','U'): return Bcn::BC5;
        case FourCc('D','X','1','0'):
            switch (dxgi) {
                case 71: case 72: return Bcn::BC1;
                case 74: case 75: return Bcn::BC2;
                case 77: case 78: return Bcn::BC3;
                case 80: return Bcn::BC4;
                case 83: return Bcn::BC5;
                case 98: case 99: return Bcn::BC7;
            }
            break;
    }
    throw std::runtime_error("DDS format is not unsigned LDR BCn");
}

void DecodeBlock(Bcn format, const uint8_t* source, uint8_t* rgba) {
    switch (format) {
        case Bcn::BC1: bcdec_bc1(source, rgba, 16); return;
        case Bcn::BC2: bcdec_bc2(source, rgba, 16); return;
        case Bcn::BC3: bcdec_bc3(source, rgba, 16); return;
        case Bcn::BC7: bcdec_bc7(source, rgba, 16); return;
        case Bcn::BC4:
        case Bcn::BC5: {
            std::array<uint8_t, 32> channels{};
            const size_t count = format == Bcn::BC4 ? 1 : 2;
            if (format == Bcn::BC4)
                bcdec_bc4(source, channels.data(), 4);
            else
                bcdec_bc5(source, channels.data(), 8);
            for (size_t i = 0; i < 16; ++i) {
                rgba[i * 4] = channels[i * count];
                rgba[i * 4 + 1] = count == 2 ? channels[i * 2 + 1] : 0;
                rgba[i * 4 + 2] = 0;
                rgba[i * 4 + 3] = 255;
            }
            return;
        }
    }
}

struct BcnDdsLayout {
    uint32_t Width = 0;
    uint32_t Height = 0;
    uint32_t MipLevels = 0;
    bool Srgb = false;
    size_t Offset = 0;
    size_t BlockBytes = 0;
    size_t MipsRgba8Bytes = 0;
    Bcn Format = Bcn::BC1;
};

BcnDdsLayout ParseBcnDds(std::span<const uint8_t> input, uint64_t fileBytes) {
    constexpr uint32_t kMaxDimension = 4096;
    constexpr size_t kHeaderSize = 128;
    if (fileBytes > kMaxBcnDdsFileBytes || fileBytes < kHeaderSize || input.size() < kHeaderSize || Read32(input, 0) != FourCc('D','D','S',' ') ||
        Read32(input, 4) != 124 || Read32(input, 76) != 32 ||
        (Read32(input, 80) & 0x4) == 0)
        throw std::runtime_error("invalid compressed DDS header");
    if ((Read32(input, 112) & (0xfe00 | 0x200000)) != 0 || Read32(input, 24) > 1)
        throw std::runtime_error("DDS cubemaps and volume textures are unsupported");

    BcnDdsLayout image;
    image.Width = Read32(input, 16);
    image.Height = Read32(input, 12);
    image.MipLevels = std::max(1U, Read32(input, 28));
    uint32_t maxMipLevels = 0;
    for (uint32_t size = std::max(image.Width, image.Height); size != 0; size >>= 1)
        ++maxMipLevels;
    if (!image.Width || !image.Height || image.Width > kMaxDimension ||
        image.Height > kMaxDimension || image.MipLevels > maxMipLevels)
        throw std::runtime_error("DDS extent or mip count is unsupported");

    const uint32_t fourCc = Read32(input, 84);
    uint32_t dxgi = 0;
    image.Offset = kHeaderSize;
    if (fourCc == FourCc('D','X','1','0')) {
        image.Offset = kMaxBcnDdsHeaderBytes;
        if (input.size() < image.Offset || fileBytes < image.Offset || Read32(input, 132) != 3 ||
            (Read32(input, 136) & 0x4) != 0 || Read32(input, 140) != 1)
            throw std::runtime_error("invalid or unsupported DX10 DDS header");
        dxgi = Read32(input, 128);
        image.Srgb = dxgi == 72 || dxgi == 75 || dxgi == 78 || dxgi == 99;
    }
    image.Format = ParseFormat(fourCc, dxgi);
    image.BlockBytes = image.Format == Bcn::BC1 || image.Format == Bcn::BC4 ? 8 : 16;

    size_t totalEncoded = 0;
    for (uint32_t level = 0; level < image.MipLevels; ++level) {
        const auto width = std::max(1U, image.Width >> level);
        const auto height = std::max(1U, image.Height >> level);
        totalEncoded += ((width + 3) / 4) * ((height + 3) / 4) * image.BlockBytes;
        if (level != 0)
            image.MipsRgba8Bytes += size_t(width) * height * 4;
    }
    if (totalEncoded > fileBytes - image.Offset)
        throw std::runtime_error("truncated DDS BCn payload");

    return image;
}

}

void ValidateBcnDds(std::span<const uint8_t> header, uint64_t fileBytes) {
    ParseBcnDds(header, fileBytes);
}

BcnDdsImage DecodeBcnDds(std::span<const uint8_t> input) {
    const auto layout = ParseBcnDds(input, input.size());
    BcnDdsImage image{layout.Width, layout.Height, layout.MipLevels, layout.Srgb, {}, {}};
    size_t offset = layout.Offset;
    image.BaseRgba8.resize(size_t(image.Width) * image.Height * 4);
    image.MipsRgba8.resize(layout.MipsRgba8Bytes);
    size_t lowerOffset = 0;
    for (uint32_t level = 0; level < image.MipLevels; ++level) {
        const auto width = std::max(1U, image.Width >> level);
        const auto height = std::max(1U, image.Height >> level);
        uint8_t* out = level == 0 ? image.BaseRgba8.data() : image.MipsRgba8.data() + lowerOffset;
        for (uint32_t y = 0; y < height; y += 4) {
            for (uint32_t x = 0; x < width; x += 4) {
                alignas(uint64_t) std::array<uint8_t, 16> compressed{};
                alignas(uint32_t) std::array<uint8_t, 64> rgba{};
                std::memcpy(compressed.data(), input.data() + offset, layout.BlockBytes);
                DecodeBlock(layout.Format, compressed.data(), rgba.data());
                offset += layout.BlockBytes;
                for (uint32_t row = 0; row < std::min(4U, height - y); ++row)
                    std::memcpy(out + (size_t(y + row) * width + x) * 4, rgba.data() + row * 16,
                                std::min(4U, width - x) * 4);
            }
        }
        if (level != 0)
            lowerOffset += size_t(width) * height * 4;
    }
    return image;
}

}
