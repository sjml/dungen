#version 300 es

uniform vec4 basic_uniforms[5];
layout(location = 0) in vec2 pos;
out vec4 col;

void main()
{
    gl_Position = mat4(basic_uniforms[1], basic_uniforms[2], basic_uniforms[3], basic_uniforms[4]) * vec4(pos, 0.0, 1.0);
    col = basic_uniforms[0];
}

