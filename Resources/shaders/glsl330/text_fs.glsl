#version 330

uniform vec4 color_data[1];
uniform sampler2D textAtlas;

in vec2 TexCoords;
layout(location = 0) out vec4 FragColor;

void main()
{
    FragColor = color_data[0] * vec4(1.0, 1.0, 1.0, texelFetch(textAtlas, ivec2(TexCoords), 0).x);
}

