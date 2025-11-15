OF_GLSL_SHADER_HEADER

// base vertex shader for lighting code

uniform mat4 modelViewProjectionMatrix;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;
in vec3 normal;

out vec3 FragPos;
out vec3 Normal; 

void main() {
    // Create model-view matrix
    mat4 modelViewMatrix = viewMatrix * worldMatrix;
    // Create normal matrix from model-view matrix
    mat4 normalMatrix = transpose(inverse(modelViewMatrix));

    // Use model-view matrix to transform vertex position to world space
    FragPos = vec3((modelViewMatrix * position).xyz); // position in view space
    Normal = normalize((normalMatrix * vec4(normal.xyz,0)).xyz); // normals in view space; note w=0
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * position;
}
