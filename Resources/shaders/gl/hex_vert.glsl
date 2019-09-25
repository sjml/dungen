#version 330 core

layout (location = 0) in vec2 hexPoint;
layout (location = 1) in vec2 worldPos;
layout (location = 2) in vec3 color;
layout (location = 3) in vec4 overlayColor;

uniform mat4 vp;
out vec3 col;

void main() {
    gl_Position = vp * vec4(hexPoint.x + worldPos.x, hexPoint.y + worldPos.y, 0.0, 1.0);
    col = mix(color, overlayColor.rgb, overlayColor.a);
}
