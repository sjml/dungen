#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct hex_vert_uniforms
{
    float4x4 matrix;
};

struct main0_out
{
    float4 col [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 hexPoint [[attribute(0)]];
    float2 worldPos [[attribute(1)]];
    float3 color [[attribute(2)]];
    float4 overlayColor [[attribute(3)]];
};

#line 18 "../Resources/shaders/base/hex.glsl"
vertex main0_out main0(main0_in in [[stage_in]], constant hex_vert_uniforms& _21 [[buffer(0)]])
{
    main0_out out = {};
#line 18 "../Resources/shaders/base/hex.glsl"
    out.gl_Position = _21.matrix * float4(in.hexPoint + in.worldPos, 0.0, 1.0);
#line 19 "../Resources/shaders/base/hex.glsl"
    out.col = float4(mix(in.color, in.overlayColor.xyz, float3(in.overlayColor.w)), 1.0);
    return out;
}

