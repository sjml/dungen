#include "stdafx.h"
#include "rendering.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "util.h"
#include "world.h"
#include "text.h"
#include "outline.h"
#include "game.h"
#include "../ui/banner.h"
#include "../ui/choice.h"
#include "../ui/tile_choice.h"
#include "../platform/platform.h"

static bool renderingInitialized = false;

static Vec2i windowDimensions;
static Vec2i framebufferDimensions;
static Vec2i orthoDimensions;

static unsigned char* screenShotBuffer;

static CameraData MainCamera;
static gbMat4 projectionMatrix;
static gbMat4 modelViewMatrix;
static gbMat4 perspectiveMatrix;
static gbMat4 orthoMatrix;

static float hexVerts[] = {
    -0.8660254f,  -0.5f,  // 1
     0.0000000f,  -1.0f,  // 2
    -0.8660254f,   0.5f,  // 0
     0.8660254f,  -0.5f,  // 3
     0.0f,         1.0f,  // 5
     0.8660254f,   0.5f,  // 4
};

static float squareVerts[] = {
    -0.5f, -0.5f,
    -0.5f,  0.5f,
     0.5f,  0.5f,
     0.5f, -0.5f,
};

static DrawState hexDraw;
static DrawState basicDraw;
static sg_pass_action passAction;

static sg_buffer tileAttribBuff;
static bool bufferUpRequested;

static Region** regions = NULL;

static const int defaultWindowWidth  = 1024;
static const int defaultWindowHeight = 768;

void InitializeRendering() {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });

    UpdateRenderingDimensions();

    sg_buffer hexVertBuff = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(hexVerts),
        .content = hexVerts,
    });

    bufferUpRequested = false;
    tileAttribBuff = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(TileDrawData) * (int)GetNumberOfTiles(),
        .usage = SG_USAGE_DYNAMIC
    });
    sg_update_buffer(tileAttribBuff, GetTileStartPointer()->draw, sizeof(TileDrawData) * (int)GetNumberOfTiles());

    hexDraw.bind = (sg_bindings){
        .vertex_buffers[0] = hexVertBuff,
        .vertex_buffers[1] = tileAttribBuff,
    };

    sds vsHex = GetShaderPath("hex_vs");
    sds fsHex = GetShaderPath("hex_fs");
    sg_shader hexShader = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] = {
            .size = sizeof(gbMat4),
            .uniforms = {
                [0] = { .name = "matrix", .type = SG_UNIFORMTYPE_MAT4},
            }
        },
        .vs.source = readTextFile(vsHex),
        .fs.source = readTextFile(fsHex),
        #ifdef SOKOL_METAL
            .vs.entry = "main0",
            .fs.entry = "main0",
        #endif
    });
    sdsfree(vsHex);
    sdsfree(fsHex);

    hexDraw.pipe = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = hexShader,
        .layout = {
            .buffers = {
                [0] = {.stride = 2 * sizeof(float)},
                [1] = {.stride = sizeof(TileDrawData), .step_func = SG_VERTEXSTEP_PER_INSTANCE}
            },
            .attrs = {
                [0] = {.buffer_index=0, .format=SG_VERTEXFORMAT_FLOAT2},
                [1] = {.buffer_index=1, .format=SG_VERTEXFORMAT_FLOAT2},
                [2] = {.buffer_index=1, .format=SG_VERTEXFORMAT_FLOAT3},
                [3] = {.buffer_index=1, .format=SG_VERTEXFORMAT_FLOAT4},
            }
        },
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP
    });

    passAction = (sg_pass_action){
        .colors[0] = { .action=SG_ACTION_CLEAR, .val={0.0f, 0.0f, 0.0f, 1.0f} }
        // .colors[0] = { .action=SG_ACTION_CLEAR, .val={1.0f, 1.0f, 1.0f, 1.0f} }
    };

    sds vsBasic = GetShaderPath("basic_vs");
    sds fsBasic = GetShaderPath("basic_fs");
    sg_shader basicShader = sg_make_shader(&(sg_shader_desc){
        .vs.uniform_blocks[0] = {
            .size = sizeof(gbVec4) + sizeof(gbMat4),
            .uniforms = {
                [0] = { .name = "basic_uniforms", .type = SG_UNIFORMTYPE_FLOAT4, .array_count = 5 },
            }
        },
        .vs.source = readTextFile(vsBasic),
        .fs.source = readTextFile(fsBasic),
        #ifdef SOKOL_METAL
            .vs.entry = "main0",
            .fs.entry = "main0",
        #endif
    });
    sdsfree(vsBasic);
    sdsfree(fsBasic);

    basicDraw.pipe = sg_make_pipeline(&(sg_pipeline_desc) {
        .shader = basicShader,
        .layout = {
            .attrs = {
                [0].format = SG_VERTEXFORMAT_FLOAT2
            }
        },
        .blend = {
            .enabled = true,
            .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
            .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA
        },
        .primitive_type = SG_PRIMITIVETYPE_TRIANGLE_STRIP
    });

//    glfwGetFramebufferSize(window, &frameW, &frameH);
//    screenShotBuffer = malloc(sizeof(GLubyte) * frameW * frameH * 3);
//    stbi_flip_vertically_on_write(1);

    InitializeText();
    LoadFont("Pixel", "fonts/04B_03__.TTF", 32.0, true);

    renderingInitialized = true;
}

void RequestRenderBufferUpdate(TileSet* ts) {
    if (!renderingInitialized || hmlen(ts) == 0) {
        return;
    }

    bufferUpRequested = true;
}

void FinalizeRendering() {
    free(screenShotBuffer);

    FinalizeText();

    #if !(DUNGEN_MOBILE)
        sg_shutdown();
        // glfwDestroyWindow(window);
        // glfwTerminate();
    #endif // !(DUNGEN_MOBILE)
}

void UpdateRenderingDimensions() {
    framebufferDimensions.x = sapp_width();
    framebufferDimensions.y = sapp_height();
    windowDimensions.x = framebufferDimensions.x / sapp_dpi_scale();
    windowDimensions.y = framebufferDimensions.y / sapp_dpi_scale();

    MainCamera.aperture = GB_MATH_PI / 2;
    MainCamera.position.x = 0.0f;
    MainCamera.position.y = 0.0f;
    MainCamera.position.z = 10.0f;
    MainCamera.view.x = 0.0f;
    MainCamera.view.y = 0.0f;
    MainCamera.view.z = -10.0f;
    MainCamera.up.x = 0.0f;
    MainCamera.up.y = 1.0f;
    MainCamera.up.z = 0.0f;
    MainCamera.zFarClip = 1.0f;
    MainCamera.zNearClip = -1.0f;
    MainCamera.windowWidth = windowDimensions.x;
    MainCamera.windowHeight = windowDimensions.y;

    float aspect = (float)MainCamera.windowWidth / (float)MainCamera.windowHeight;
    gb_mat4_perspective(&projectionMatrix, MainCamera.aperture, aspect, MainCamera.zNearClip, MainCamera.zFarClip);
    gb_mat4_look_at(&modelViewMatrix, MainCamera.position, MainCamera.view, MainCamera.up);
    gb_mat4_mul(&perspectiveMatrix, &projectionMatrix, &modelViewMatrix);

    gbVec2 scaleDims;
    scaleDims.x = GetTileDimensions().y * 0.5f;
    scaleDims.y = scaleDims.x;
    hexVerts[ 0] *= scaleDims.x; hexVerts[ 1] *= scaleDims.y;
    hexVerts[ 2] *= scaleDims.x; hexVerts[ 3] *= scaleDims.y;
    hexVerts[ 4] *= scaleDims.x; hexVerts[ 5] *= scaleDims.y;
    hexVerts[ 6] *= scaleDims.x; hexVerts[ 7] *= scaleDims.y;
    hexVerts[ 8] *= scaleDims.x; hexVerts[ 9] *= scaleDims.y;
    hexVerts[10] *= scaleDims.x; hexVerts[11] *= scaleDims.y;

    orthoDimensions.y = defaultWindowHeight;
    orthoDimensions.x = (int)((float)defaultWindowHeight * aspect);
    gb_mat4_ortho2d(&orthoMatrix, 0.0f, (float)orthoDimensions.x, (float)orthoDimensions.y, 0.0f);
}

Vec2i GetWindowDimensions(void) {
    return windowDimensions;
}

Vec2i GetFramebufferDimensions(void) {
    return framebufferDimensions;
}

Vec2i GetOrthoDimensions(void) {
    return orthoDimensions;
}

void DumpScreenShot(const char* fileName) {
//    glReadPixels(0, 0, frameW, frameH, GL_RGB, GL_UNSIGNED_BYTE, screenShotBuffer);
//    stbi_write_png(fileName, frameW, frameH, 3, screenShotBuffer, frameW * 3);
}

void AddRegionToRendering(Region* r) {
    arrpush(regions, r);
}

void RemoveRegionFromRendering(Region* r) {
    for (int i = 0; i < arrlen(regions); i++) {
        if (regions[i] == r) {
            arrdel(regions, i);
            return;
        }
    }
    fprintf(stderr, "ERROR: removing Region that was not part of the world.\n");
}

Region** GetRenderingRegions() {
    return regions;
}

gbVec2 WorldToScreen(gbVec2 worldCoordinates) {
    gbVec4 wc = {worldCoordinates.x, worldCoordinates.y, 0.0f, 1.0f};
    gbVec4 clip;
    gb_mat4_mul_vec4(&clip, &projectionMatrix, wc);
    gb_mat4_mul_vec4(&clip, &perspectiveMatrix, wc);
    gbVec3 ndc;
    gb_vec3_div(&ndc, clip.xyz, clip.w);

    gbVec2 viewOffset = { 0.0f, 0.0f };

    gbVec2 spos = {
            (
                (ndc.x + 1.0f) / 2.0f
            ) * windowDimensions.x + viewOffset.x,
            (
                (1.0f - ndc.y) / 2.0f
            ) * windowDimensions.y + viewOffset.y
    };

    return spos;
}

gbVec2 ScreenToWorld(gbVec2 screenCoordinates) {
    gbMat4 persp, final;
    gb_mat4_mul(&persp, &projectionMatrix, &modelViewMatrix);
    gb_mat4_inverse(&final, &persp);
    gbVec4 in = {
        screenCoordinates.x, screenCoordinates.y, 0.0f, 1.0f
    };

    gbVec2 viewOffset = { 0.0f, 0.0f };

    in.x = (in.x - viewOffset.x) / windowDimensions.x;
    in.y = (in.y - viewOffset.y) / windowDimensions.y;

    in.x = in.x * 2 - 1;
    in.y = in.y * 2 - 1;
    in.z = in.z * 2 - 1;

    gbVec4 out;
    gb_mat4_mul_vec4(&out, &final, in);
    out.x /= out.w;
    out.y /= out.w;
    out.z /= out.w;

    float multFactor = 10.0f; // TODO: this shouldn't be necessary :-/
    gbVec2 ret = {out.x * -multFactor, out.y * multFactor};
    return ret;
}

gbVec2 ScreenToOrtho(gbVec2 screenCoordinates) {
    float xPerc = screenCoordinates.x / windowDimensions.x;
    float yPerc = screenCoordinates.y / windowDimensions.y;

    gbVec2 ret = {
        xPerc * orthoDimensions.x,
        yPerc * orthoDimensions.y
    };
    return ret;
}

void DrawShapeBuffer(sg_buffer buff, int numPoints, gbVec4 color, gbMat4 *matrix) {
    sg_apply_pipeline(basicDraw.pipe);
    BasicUniforms bu;
    bu.matrix = *matrix;
    bu.color = color;
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &bu, sizeof(BasicUniforms));
    sg_apply_bindings(&(sg_bindings){
        .vertex_buffers[0] = buff,
    });
    sg_draw(0, numPoints, 1);
}

int Render() {
    if (bufferUpRequested) {
        // SOKOL TODO: figure out how to do partial updates of a buffer
        sg_update_buffer(tileAttribBuff, GetTileStartPointer()->draw, sizeof(TileDrawData) * (int)GetNumberOfTiles());
        bufferUpRequested = false;
    }

    sg_begin_default_pass(&passAction, framebufferDimensions.x, framebufferDimensions.y);

    sg_apply_pipeline(hexDraw.pipe);
    sg_apply_bindings(&hexDraw.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &perspectiveMatrix, sizeof(gbMat4));
    sg_draw(0, 6, (int)GetNumberOfTiles());

    sg_apply_pipeline(basicDraw.pipe);
    BasicUniforms bu;
    bu.matrix = perspectiveMatrix;
    for (long ri=0; ri < arrlen(regions); ri++) {
        if (regions[ri]->outline != NULL) {
            sg_apply_bindings(&(regions[ri]->outline->bindings));
            bu.color = regions[ri]->outline->color;
            sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, &bu, sizeof(BasicUniforms));
            sg_draw(0, (int)regions[ri]->outline->numPoints, 1);
        }
    }

    PrepDrawText(&orthoMatrix);
    for (int i=0; i < arrlen(regions); i++) {
        if (regions[i]->label != NULL) {
            DrawText(regions[i]->label);
        }
    }
    FinishDrawText();

    RenderBanners(&orthoMatrix);

    if (GetChoiceStatus() >= 0) {
        RenderChoices(&orthoMatrix);
    }
    if (GetTileChoiceStatus() >= 0) {
        RenderTileChoice();
    }


    sg_end_pass();
    sg_commit();

     return 0;
}
