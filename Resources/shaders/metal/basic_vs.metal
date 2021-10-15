#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct basic_uniforms
{
    float4 color;
    float4x4 matrix;
};

struct main0_out
{
    float4 col [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float2 pos [[attribute(0)]];
};

#line 16 "../Resources/shaders/base/basic.glsl"
vertex main0_out main0(main0_in in [[stage_in]], constant basic_uniforms& _21 [[buffer(0)]])
{
    main0_out out = {};
#line 16 "../Resources/shaders/base/basic.glsl"
    out.gl_Position = _21.matrix * float4(in.pos, 0.0, 1.0);
#line 17 "../Resources/shaders/base/basic.glsl"
    out.col = _21.color;
    return out;
}

