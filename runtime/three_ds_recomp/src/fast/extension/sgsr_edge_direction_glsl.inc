// Adapted from sgsr/v1/include/glsl/sgsr1_shader_mobile_edge_direction.frag.
// Copyright (c) 2025, Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// Full terms in LICENSES/Snapdragon-GSR-BSD-3-Clause.txt.

R"glsl(
vec2 sgsr_weight(vec2 offset, float green, vec3 data) {
    float edgeDistance = dot(offset, data.zy);
    float radius = dot(offset, offset) + edgeDistance * edgeDistance *
        (clamp(green * green * data.x, 0.0, 1.0) * 0.7 - 1.0);
    float a = radius - 4.0;
    float weight = (radius * a - a) * a * a;
    return vec2(weight, weight * green);
}

vec4 sgsr_edge_direction(vec2 uv, vec4 color) {
    vec2 size = vec2(textureSize(physical_scanout, 0));
    vec2 texel = 1.0 / size;
    vec2 pixel = uv * size + vec2(-0.5, 0.5);
    vec2 grid = floor(pixel);
    vec2 coord = grid * texel;
    vec2 p = pixel - grid;
    // RGBA mode uses green as the luminance proxy. The existing linear
    // clamp-to-edge scanout sampler supplies the correct image boundaries.
    vec4 left = textureGather(physical_scanout, coord, 1);
    float vote = abs(left.z - left.y) + abs(color.g - left.y) + abs(color.g - left.z);
    if (vote <= 8.0 / 255.0) return color;

    coord.x += texel.x;
    vec4 right = textureGather(physical_scanout, coord + vec2(texel.x, 0), 1);
    vec4 vertical = vec4(
        textureGather(physical_scanout, coord - vec2(0, texel.y), 1).wz,
        textureGather(physical_scanout, coord + vec2(0, texel.y), 1).yx);
    float mean = (left.y + left.z + right.x + right.w) * 0.25;
    left -= mean;
    right -= mean;
    vertical -= mean;
    float variation = dot(abs(left) + abs(right) + abs(vertical), vec4(1));
    if (variation <= 0.000001) return color;

    vec2 gradient = vec2(right.x - left.z, right.w - left.y);
    vec2 direction = vec2(gradient.x + gradient.y, gradient.x - gradient.y);
    direction *= inversesqrt(dot(direction, direction) + 3.075740e-05);
    float norm = 1.014185e+01 / variation;
    vec3 data = vec3(norm * norm, direction);
    vec2 weights = sgsr_weight(p + vec2( 0,  1), vertical.x, data)
                 + sgsr_weight(p + vec2(-1,  1), vertical.y, data)
                 + sgsr_weight(p + vec2(-1, -2), vertical.z, data)
                 + sgsr_weight(p + vec2( 0, -2), vertical.w, data)
                 + sgsr_weight(p + vec2( 1, -1), left.x, data)
                 + sgsr_weight(p + vec2( 0, -1), left.y, data)
                 + sgsr_weight(p,                left.z, data)
                 + sgsr_weight(p + vec2( 1,  0), left.w, data)
                 + sgsr_weight(p + vec2(-1, -1), right.x, data)
                 + sgsr_weight(p + vec2(-2, -1), right.y, data)
                 + sgsr_weight(p + vec2(-2,  0), right.z, data)
                 + sgsr_weight(p + vec2(-1,  0), right.w, data);
    if (abs(weights.x) <= 0.000001) return color;

    float darkest = min(min(left.y, left.z), min(right.x, right.w));
    float brightest = max(max(left.y, left.z), max(right.x, right.w));
    float green = clamp(2.0 * weights.y / weights.x, darkest, brightest) + mean;
    float delta = clamp(green - color.g, -23.0 / 255.0, 23.0 / 255.0);
    return vec4(clamp(color.rgb + delta, 0.0, 1.0), color.a);
}
)glsl"
