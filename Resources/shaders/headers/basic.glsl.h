#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Overview:

        Shader program 'basic':
            Get shader desc: basic_shader_desc()
            Vertex shader: basic_vert
                Attribute slots:
                    ATTR_basic_vert_pos = 0
                Uniform block 'basic_uniforms':
                    C struct: basic_uniforms_t
                    Bind slot: SLOT_basic_uniforms = 0
            Fragment shader: basic_frag


    Shader descriptor structs:

        sg_shader basic = sg_make_shader(basic_shader_desc());

    Vertex attribute locations for vertex shader 'basic_vert':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_basic_vert_pos] = { ... },
                },
            },
            ...});

    Image bind slots, use as index in sg_bindings.vs_images[] or .fs_images[]


    Bind slot and C-struct for uniform block 'basic_uniforms':

        basic_uniforms_t basic_uniforms = {
            .color = ...;
            .matrix = ...;
        };
        sg_apply_uniforms(SG_SHADERSTAGE_[VS|FS], SLOT_basic_uniforms, &basic_uniforms, sizeof(basic_uniforms));

*/
#include <stdint.h>
#include <stdbool.h>
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#define ATTR_basic_vert_pos (0)
#define SLOT_basic_uniforms (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct basic_uniforms_t {
    gbVec4 color;
    gbMat4 matrix;
} basic_uniforms_t;
#pragma pack(pop)
/*
    #version 330
    
    uniform vec4 basic_uniforms[5];
    layout(location = 0) in vec2 pos;
    out vec4 col;
    
    void main()
    {
        gl_Position = mat4(basic_uniforms[1], basic_uniforms[2], basic_uniforms[3], basic_uniforms[4]) * vec4(pos, 0.0, 1.0);
        col = basic_uniforms[0];
    }
    
*/
static const char basic_vert_source_glsl330[264] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x62,0x61,0x73,0x69,0x63,
    0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x35,0x5d,0x3b,0x0a,0x6c,0x61,
    0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
    0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x70,0x6f,0x73,0x3b,0x0a,
    0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x3b,0x0a,0x0a,0x76,
    0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,
    0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x6d,
    0x61,0x74,0x34,0x28,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,
    0x6d,0x73,0x5b,0x31,0x5d,0x2c,0x20,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,
    0x66,0x6f,0x72,0x6d,0x73,0x5b,0x32,0x5d,0x2c,0x20,0x62,0x61,0x73,0x69,0x63,0x5f,
    0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x33,0x5d,0x2c,0x20,0x62,0x61,0x73,
    0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x34,0x5d,0x29,0x20,
    0x2a,0x20,0x76,0x65,0x63,0x34,0x28,0x70,0x6f,0x73,0x2c,0x20,0x30,0x2e,0x30,0x2c,
    0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x20,0x3d,
    0x20,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,
    0x30,0x5d,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 330
    
    layout(location = 0) out vec4 FragColor;
    in vec4 col;
    
    void main()
    {
        FragColor = col;
    }
    
*/
static const char basic_frag_source_glsl330[108] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x6c,0x61,
    0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
    0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x46,0x72,0x61,0x67,
    0x43,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,
    0x6f,0x6c,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,
    0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,
    0x20,0x3d,0x20,0x63,0x6f,0x6c,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 300 es
    
    uniform vec4 basic_uniforms[5];
    layout(location = 0) in vec2 pos;
    out vec4 col;
    
    void main()
    {
        gl_Position = mat4(basic_uniforms[1], basic_uniforms[2], basic_uniforms[3], basic_uniforms[4]) * vec4(pos, 0.0, 1.0);
        col = basic_uniforms[0];
    }
    
*/
static const char basic_vert_source_glsl300es[267] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x0a,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x62,0x61,
    0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x35,0x5d,0x3b,
    0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,
    0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x70,0x6f,
    0x73,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x3b,
    0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,
    0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,
    0x3d,0x20,0x6d,0x61,0x74,0x34,0x28,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,
    0x66,0x6f,0x72,0x6d,0x73,0x5b,0x31,0x5d,0x2c,0x20,0x62,0x61,0x73,0x69,0x63,0x5f,
    0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x32,0x5d,0x2c,0x20,0x62,0x61,0x73,
    0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x33,0x5d,0x2c,0x20,
    0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x5b,0x34,
    0x5d,0x29,0x20,0x2a,0x20,0x76,0x65,0x63,0x34,0x28,0x70,0x6f,0x73,0x2c,0x20,0x30,
    0x2e,0x30,0x2c,0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,
    0x6c,0x20,0x3d,0x20,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,
    0x6d,0x73,0x5b,0x30,0x5d,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 300 es
    precision mediump float;
    precision highp int;
    
    layout(location = 0) out highp vec4 FragColor;
    in highp vec4 col;
    
    void main()
    {
        FragColor = col;
    }
    
*/
static const char basic_frag_source_glsl300es[169] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x30,0x30,0x20,0x65,0x73,0x0a,
    0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,
    0x70,0x20,0x66,0x6c,0x6f,0x61,0x74,0x3b,0x0a,0x70,0x72,0x65,0x63,0x69,0x73,0x69,
    0x6f,0x6e,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x69,0x6e,0x74,0x3b,0x0a,0x0a,0x6c,
    0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,
    0x20,0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,
    0x63,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x69,0x6e,
    0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x3b,
    0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,
    0x20,0x20,0x20,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,
    0x63,0x6f,0x6c,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
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
    
    #line 16 ""
    vertex main0_out main0(main0_in in [[stage_in]], constant basic_uniforms& _20 [[buffer(0)]])
    {
        main0_out out = {};
    #line 16 ""
        out.gl_Position = _20.matrix * float4(in.pos, 0.0, 1.0);
    #line 17 ""
        out.col = _20.color;
        return out;
    }
    
*/
static const char basic_vert_source_metal_macos[547] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x62,
    0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x73,0x0a,0x7b,0x0a,
    0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x78,0x34,0x20,0x6d,
    0x61,0x74,0x72,0x69,0x78,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,
    0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,
    0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x20,0x5b,0x5b,0x75,
    0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,
    0x74,0x69,0x6f,0x6e,0x20,0x5b,0x5b,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x5d,
    0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,
    0x69,0x6e,0x30,0x5f,0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
    0x61,0x74,0x32,0x20,0x70,0x6f,0x73,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,
    0x75,0x74,0x65,0x28,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,
    0x69,0x6e,0x65,0x20,0x31,0x36,0x20,0x22,0x22,0x0a,0x76,0x65,0x72,0x74,0x65,0x78,
    0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,
    0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,0x5b,0x5b,0x73,
    0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x2c,0x20,0x63,0x6f,0x6e,0x73,0x74,
    0x61,0x6e,0x74,0x20,0x62,0x61,0x73,0x69,0x63,0x5f,0x75,0x6e,0x69,0x66,0x6f,0x72,
    0x6d,0x73,0x26,0x20,0x5f,0x32,0x30,0x20,0x5b,0x5b,0x62,0x75,0x66,0x66,0x65,0x72,
    0x28,0x30,0x29,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x6d,0x61,0x69,
    0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,0x3d,0x20,0x7b,0x7d,0x3b,
    0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x31,0x36,0x20,0x22,0x22,0x0a,0x20,0x20,0x20,
    0x20,0x6f,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,
    0x20,0x3d,0x20,0x5f,0x32,0x30,0x2e,0x6d,0x61,0x74,0x72,0x69,0x78,0x20,0x2a,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x34,0x28,0x69,0x6e,0x2e,0x70,0x6f,0x73,0x2c,0x20,0x30,
    0x2e,0x30,0x2c,0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,
    0x31,0x37,0x20,0x22,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,0x63,0x6f,
    0x6c,0x20,0x3d,0x20,0x5f,0x32,0x30,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,
    0x0a,0x0a,0x00,
};
/*
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
    
*/
static const char basic_frag_source_metal_macos[333] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,
    0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,
    0x5b,0x5b,0x63,0x6f,0x6c,0x6f,0x72,0x28,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,
    0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,
    0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,
    0x6f,0x6c,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x30,0x29,
    0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x31,0x31,
    0x20,0x22,0x22,0x0a,0x66,0x72,0x61,0x67,0x6d,0x65,0x6e,0x74,0x20,0x6d,0x61,0x69,
    0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x28,0x6d,0x61,0x69,
    0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,0x5b,0x5b,0x73,0x74,0x61,0x67,0x65,
    0x5f,0x69,0x6e,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x6d,0x61,0x69,
    0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,0x3d,0x20,0x7b,0x7d,0x3b,
    0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x31,0x31,0x20,0x22,0x22,0x0a,0x20,0x20,0x20,
    0x20,0x6f,0x75,0x74,0x2e,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,
    0x20,0x69,0x6e,0x2e,0x63,0x6f,0x6c,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,
    0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
#if !defined(SOKOL_GFX_INCLUDED)
  #error "Please include sokol_gfx.h before basic.glsl.h"
#endif
static inline const sg_shader_desc* basic_shader_desc(void) {
  if (sg_query_backend() == SG_BACKEND_GLCORE33) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "pos";
      desc.vs.source = basic_vert_source_glsl330;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 80;
      desc.vs.uniform_blocks[0].uniforms[0].name = "basic_uniforms";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
      desc.fs.source = basic_frag_source_glsl330;
      desc.fs.entry = "main";
      desc.label = "basic_shader";
    };
    return &desc;
  }
  if (sg_query_backend() == SG_BACKEND_GLES3) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "pos";
      desc.vs.source = basic_vert_source_glsl300es;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 80;
      desc.vs.uniform_blocks[0].uniforms[0].name = "basic_uniforms";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 5;
      desc.fs.source = basic_frag_source_glsl300es;
      desc.fs.entry = "main";
      desc.label = "basic_shader";
    };
    return &desc;
  }
  if (sg_query_backend() == SG_BACKEND_METAL_MACOS) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.vs.source = basic_vert_source_metal_macos;
      desc.vs.entry = "main0";
      desc.vs.uniform_blocks[0].size = 80;
      desc.fs.source = basic_frag_source_metal_macos;
      desc.fs.entry = "main0";
      desc.label = "basic_shader";
    };
    return &desc;
  }
  return 0;
}
