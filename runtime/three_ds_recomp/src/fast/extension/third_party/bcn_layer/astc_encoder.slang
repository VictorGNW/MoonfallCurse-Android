// MIT License
// Copyright (c) 2026 Lee Gao

import astc_pack;
import encoder_utils;

#ifdef ENABLE_DEBUG
import astc_debug;
#endif

// #define UNROLL_SOLVE_WEIGHTS
#define UNROLL_ENCODE_AABB

[[vk::binding(0, 0)]]
StructuredBuffer<uint> input_rgba8_buffer;

[[vk::binding(1, 0)]]
RWStructuredBuffer<uint4> astc_blocks;

struct PushConstants {
  int2 u_TextureDimensions;
};

[[vk::push_constant]]
PushConstants pushConstants;

static uint buffer_index;
static uint8_t4 pixels[16];

extension uint8_t4 {
  half4 to_unorm() { return half4(this) / 255.0h; }
}

void solve_weights(half4 ep0, half4 ep1, out uint8_t weights[16]) {
  let L = ep1 - ep0;
  let lenSq = max(dot(L, L), 0.00006103515625h);
  for (int i = 0; i < 16; i++) {
#ifndef UNROLL_SOLVE_WEIGHTS
    // 3x + 1 mod 3 is always 1, but our compilers aren't smart enough to figure
    // this out yet
    // This acts as an optimization barrier to keep the mali compiler from
    // unrolling this loop, which blows up our register file and forces a stack
    // spill
    let always_true = bool(((pushConstants.u_TextureDimensions.x * 3) + 1) % 3);
    if (!always_true) {
      continue;
    }
#endif
    let C = pixels[i].to_unorm();
    let pDotL = dot(C - ep0, L);
    let w = pDotL / lenSq;
    weights[i] = uint8_t(saturate(w) * 255u);
  }
}

void encode_block_AABB(out uint8_t4 endpoint_start, out uint8_t4 endpoint_end,
                       out uint8_t weights[16]) {
  endpoint_start = uint8_t4(255u);
  endpoint_end = uint8_t4(0u);
  for (int i = 0; i < 16; i += 1) {
#ifndef UNROLL_ENCODE_AABB
    let always_true = bool(((pushConstants.u_TextureDimensions.x * 3) + 1) % 3);
    if (!always_true) {
      continue;
    }
#endif
    endpoint_start = min(endpoint_start, pixels[i]);
    endpoint_end = max(endpoint_end, pixels[i]);
  }

  var ep0 = half4(endpoint_start) / 255.0h;
  var ep1 = half4(endpoint_end) / 255.0h;
  solve_weights(ep0, ep1, weights);
}

[shader("compute")]
[numthreads(8, 8, 1)]
void main(uint3 gl_GlobalInvocationID: SV_DispatchThreadID) {
  int2 block_pos = int2(gl_GlobalInvocationID.xy);
  int2 tex_coord = block_pos * 4;

#ifdef ENABLE_DIAGNOSTICS
  uint2 startTime = clockRealtime2x32EXT();
#endif

  if (any(tex_coord >= pushConstants.u_TextureDimensions))
    return;

  // Partial blocks repeat the last texel at each edge, including mip tails.
  buffer_index =
      block_pos.y * ((pushConstants.u_TextureDimensions.x + 3) / 4) + block_pos.x;

  bool use_alpha = false;
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      let coord = min(tex_coord + int2(x, y), pushConstants.u_TextureDimensions - 1);
      let pixel = input_rgba8_buffer[coord.y * pushConstants.u_TextureDimensions.x + coord.x];
      uint r = (pixel >> 0) & 0xFFu;
      uint g = (pixel >> 8) & 0xFFu;
      uint b = (pixel >> 16) & 0xFFu;
      uint a = (pixel >> 24) & 0xFFu;

      pixels[y * 4 + x] =
          uint8_t4(uint8_t(r), uint8_t(g), uint8_t(b), uint8_t(a));
      if (a < 255)
        use_alpha = true;
    }
  }

  uint8_t4 ep0, ep1;
  uint8_t weights_1p[16];
  uint8_t4 eps[2] = {};
  uint4 packed_1p;

  // TODO(leegao): look for bit tricks for the 1-partition AABB case
  encode_block_AABB(ep0, ep1, weights_1p);
  eps[0] = uint8_t4(ep0);
  eps[1] = uint8_t4(ep1);
  // TODO(leegao): this is the bottleneck
  packed_1p = pack_astc_4x4_block(eps, weights_1p, use_alpha);
  astc_blocks[buffer_index] = packed_1p;

#ifdef ENABLE_DEBUG
  log_debug(buffer_index, ep0, ep1, weights_1p, pixels, use_alpha);
#endif
}
