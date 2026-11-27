#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace Oot3d::Renderer {

inline constexpr uint64_t kMaxBcnDdsFileBytes = 23U * 1024 * 1024;
inline constexpr std::size_t kMaxBcnDdsHeaderBytes = 148;

void ValidateBcnDds(std::span<const uint8_t> header, uint64_t fileBytes);

struct BcnDdsImage {
    uint32_t Width = 0;
    uint32_t Height = 0;
    uint32_t MipLevels = 1;
    bool Srgb = false;
    std::vector<uint8_t> BaseRgba8;
    std::vector<uint8_t> MipsRgba8;
};

// Reads a single 2D DDS image with unsigned LDR BC1/2/3/4/5/7 blocks.
// The resulting rows retain the orientation of the DDS.
BcnDdsImage DecodeBcnDds(std::span<const uint8_t> input);

}
