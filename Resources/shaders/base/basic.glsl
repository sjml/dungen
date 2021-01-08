@ctype vec4 gbVec4
@ctype mat4 gbMat4

@vs basic_vert
layout (location = 0) in vec2 pos;

uniform basic_uniforms {
    vec4 color;
    mat4 matrix;
};

out vec4 col;

void main() {
    gl_Position = matrix * vec4(pos, 0.0, 1.0);
    col = color;
}
@end

@fs basic_frag
out vec4 FragColor;

in vec4 col;

void main() {
    FragColor = col;
}
@end

@program basic basic_vert basic_frag
