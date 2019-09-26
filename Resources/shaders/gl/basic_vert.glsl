#version 330 core

layout (location = 0) in vec2 pos;

uniform vec4 color;
uniform mat4 vp;

out vec4 col;

void main() {
    gl_Position = vp * vec4(pos.x, pos.y, 0.0, 1.0);
    col = color;
}
