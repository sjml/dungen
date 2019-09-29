#version 300 es

precision mediump float;

uniform sampler2D textAtlas;
uniform vec4 textColor;

in vec2 TexCoords;

out vec4 FragColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texelFetch(textAtlas, ivec2(TexCoords.xy), 0).a);
    FragColor = textColor * sampled;
}
