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

#line 17 ""
fragment main0_out main0(main0_in in [[stage_in]], constant text_frag_uniforms& _35 [[buffer(0)]], texture2d<float> textAtlas [[texture(0)]], sampler textAtlasSmplr [[sampler(0)]])
{
    main0_out out = {};
#line 17 ""
#line 18 ""
    out.FragColor = _35.textColor * float4(1.0, 1.0, 1.0, textAtlas.read(uint2(int2(in.TexCoords)), 0).x);
    return out;
}

