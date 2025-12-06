OF_GLSL_SHADER_HEADER

// base vertex shader for lighting code

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;

out vec3 FragPos;

void main() {
    FragPos = vec3((viewMatrix * worldMatrix * position).xyz);
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * position;
}
