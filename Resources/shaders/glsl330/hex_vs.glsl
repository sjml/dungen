#version 330

uniform vec4 hex_vert_uniforms[4];
layout(location = 0) in vec2 hexPoint;
layout(location = 1) in vec2 worldPos;
out vec4 col;
layout(location = 2) in vec3 color;
layout(location = 3) in vec4 overlayColor;

void main()
{
    gl_Position = mat4(hex_vert_uniforms[0], hex_vert_uniforms[1], hex_vert_uniforms[2], hex_vert_uniforms[3]) * vec4(hexPoint + worldPos, 0.0, 1.0);
    col = vec4(mix(color, overlayColor.xyz, vec3(overlayColor.w)), 1.0);
}

