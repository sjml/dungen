#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in vertData {
    vec4 center;
    vec3 col;
} hex[];

out vec3 col;

uniform mat4 vp;
uniform mat4 model;

void main() {
    col = hex[0].col;

    //1
    gl_Position = hex[0].center + vp * model * vec4( -0.8660254f,  -0.5f, 0.0, 0.0);
    EmitVertex();

    //2
    gl_Position = hex[0].center + vp * model * vec4(  0.0000000f,  -1.0f, 0.0, 0.0);
    EmitVertex();

    //0
    gl_Position = hex[0].center + vp * model * vec4( -0.8660254f,   0.5f, 0.0, 0.0);
    EmitVertex();

    //3
    gl_Position = hex[0].center + vp * model * vec4(  0.8660254f,  -0.5f, 0.0, 0.0);
    EmitVertex();

    //5
    gl_Position = hex[0].center + vp * model * vec4(  0.0f,         1.0f, 0.0, 0.0);
    EmitVertex();

    //4
    gl_Position = hex[0].center + vp * model * vec4(  0.8660254f,   0.5f, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}
