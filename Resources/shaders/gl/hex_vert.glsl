#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec4 overlay;

uniform mat4 vp;

out vertData {
    vec4 center;
    vec3 col;
} hex;

void main() {
    hex.center = vp * vec4(pos.x, pos.y, 0.0, 1.0);
    hex.col = mix(color, overlay.rgb, overlay.a);
}
