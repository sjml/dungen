#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 FragColor [[color(0)]];
};

struct main0_in
{
    float4 col [[user(locn0)]];
};

#line 11 ""
fragment main0_out main0(main0_in in [[stage_in]])
{
    main0_out out = {};
#line 11 ""
    out.FragColor = in.col;
    return out;
}

