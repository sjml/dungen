@vs hex_vert
layout (location = 0) in vec2 hexPoint;
layout (location = 1) in vec2 worldPos;
layout (location = 2) in vec3 color;
layout (location = 3) in vec4 overlayColor;

uniform mat4 vp;
out vec4 col;

void main() {
    gl_Position = vp * vec4(hexPoint.xy + worldPos.xy, 0.0, 1.0);
    col = vec4(mix(color, overlayColor.rgb, overlayColor.a), 1.0);
}
@end

@fs hex_frag
out vec4 FragColor;

in vec4 col;

void main() {
    FragColor = col;
}
@end

@program hex hex_vert hex_frag
