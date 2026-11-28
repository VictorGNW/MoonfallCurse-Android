# ASTC encoder provenance

The three Slang sources are based on
[leegao/bcn_layer shader-v3](https://github.com/leegao/bcn_layer/tree/88b682777d6a9cae1f8787edd3cc1a1ebd98527b)
by Lee Gao, under the adjacent MIT license. The Vulkan layer itself is
independent of this integration. The encoder here takes tightly packed RGBA8,
clamps partial ASTC blocks to the edges of each mip and bounds the FP16 weight
denominator for uniform blocks. It keeps the upstream 4×4 LDR block packer.

The committed SPIR-V is embedded by the Android Vulkan build. To regenerate
it in this directory, use Slang 2026.23 and the shader tools from Android NDK r29.

```sh
slangc astc_encoder.slang -entry main -target glsl -line-directive-mode none \
    -D DISABLE_RECONSTRUCTION -o /tmp/moonfall-astc.comp
glslc --target-env=vulkan1.2 -O /tmp/moonfall-astc.comp -o astc_encoder.spv
spirv-val --target-env vulkan1.2 astc_encoder.spv
```

The SPIR-V declares Float16 and Int8. Both descriptor bindings are standard
32-bit storage buffers and the push constant contains two 32-bit dimensions.
