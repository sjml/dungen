#version 300 es
precision mediump float;
precision highp int;

layout(location = 0) out highp vec4 FragColor;
in highp vec4 col;

void main()
{
    FragColor = col;
}

