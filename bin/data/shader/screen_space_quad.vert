#version 150

in vec4 position;
in vec2 texcoord;

out vec2 uv0;

uniform mat4 modelViewProjectionMatrix; // auto-set by oF

void main() {
    uv0 = texcoord;
    gl_Position = modelViewProjectionMatrix*position; // transformed vertex postion, as usual
}
