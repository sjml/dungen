#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct text_frag_uniforms
{
    float4 textColor;
};

struct main0_out
{
    float4 FragColor [[color(0)]];
};

struct main0_in
{
    float2 TexCoords [[user(locn0)]];
};

#line 17 "../Resources/shaders/base/text.glsl"
fragment main0_out main0(main0_in in [[stage_in]], constant text_frag_uniforms& _36 [[buffer(0)]], texture2d<float> textAtlas [[texture(0)]], sampler textAtlasSmplr [[sampler(0)]])
{
    main0_out out = {};
#line 17 "../Resources/shaders/base/text.glsl"
#line 18 "../Resources/shaders/base/text.glsl"
    out.FragColor = _36.textColor * float4(1.0, 1.0, 1.0, textAtlas.read(uint2(int2(in.TexCoords)), 0).x);
    return out;
}

