#version 300 es

uniform vec4 text_vert_uniforms[4];
layout(location = 0) in vec4 vertex;
out vec2 TexCoords;

void main()
{
    gl_Position = mat4(text_vert_uniforms[0], text_vert_uniforms[1], text_vert_uniforms[2], text_vert_uniforms[3]) * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}

