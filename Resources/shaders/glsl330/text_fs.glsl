#version 330

uniform vec4 text_frag_uniforms[1];
uniform sampler2D textAtlas;

in vec2 TexCoords;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = text_frag_uniforms[0] * vec4(1.0, 1.0, 1.0, texelFetch(textAtlas, ivec2(TexCoords), 0).x);
}

