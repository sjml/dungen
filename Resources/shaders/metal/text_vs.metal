#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float2 TexCoords [[user(locn0)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float4 vertex0 [[attribute(0)]];
};

#line 13 ""
vertex main0_out main0(main0_in in [[stage_in]], constant float4x4& ortho [[buffer(0)]])
{
    main0_out out = {};
#line 13 ""
    out.gl_Position = ortho * float4(in.vertex0.xy, 0.0, 1.0);
    out.TexCoords = in.vertex0.zw;
    return out;
}

