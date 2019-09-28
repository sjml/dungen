#version 330 core

layout (location = 0) in vec4 vertex;

uniform mat4 ortho;

out vec2 TexCoords;

void main() {
    gl_Position = ortho * vec4(vertex.x, vertex.y, 0.0, 1.0);
    TexCoords = vertex.zw;
}
