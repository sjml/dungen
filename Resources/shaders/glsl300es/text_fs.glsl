#version 300 es
precision mediump float;
precision highp int;

uniform highp vec4 text_frag_uniforms[1];
uniform highp sampler2D textAtlas;

in highp vec2 TexCoords;
layout(location = 0) out highp vec4 FragColor;

void main()
{
    FragColor = text_frag_uniforms[0] * vec4(1.0, 1.0, 1.0, texelFetch(textAtlas, ivec2(TexCoords), 0).x);
}

