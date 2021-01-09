#version 300 es

uniform mat4 matrix;

layout(location = 0) in vec2 hexPoint;
layout(location = 1) in vec2 worldPos;
out vec4 col;
layout(location = 2) in vec3 color;
layout(location = 3) in vec4 overlayColor;

void main()
{
    gl_Position = matrix * vec4(hexPoint + worldPos, 0.0, 1.0);
    col = vec4(mix(color, overlayColor.xyz, vec3(overlayColor.w)), 1.0);
}

