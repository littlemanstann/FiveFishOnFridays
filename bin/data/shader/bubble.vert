#version 150
in vec3 position;
in float alpha;

uniform mat4 MVP;
out float vAlpha;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    vAlpha = alpha;
}
