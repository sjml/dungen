@vs text_vert
layout (location = 0) in vec4 vertex;

uniform mat4 ortho;

out vec2 TexCoords;

void main() {
    gl_Position = ortho * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
@end

@fs text_frag
uniform sampler2D textAtlas;

uniform color_data {
    vec4 textColor;
};

in vec2 TexCoords;

out vec4 FragColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texelFetch(textAtlas, ivec2(TexCoords.xy), 0).r);
    FragColor = textColor * sampled;
}
@end

@program text text_vert text_frag
