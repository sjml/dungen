#version 330

layout(location = 0) out vec4 FragColor;
in vec4 col;

void main()
{
    FragColor = col;
}

