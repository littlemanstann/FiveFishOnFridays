OF_GLSL_SHADER_HEADER

// base vertex shader for lighting code

uniform mat4 modelViewProjectionMatrix;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec4 position;
in vec3 normal;
in vec2 texcoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
	TexCoord = texcoord;
	// added viewMatrix into the transformation to bring both the frags and normals to view space
    FragPos = vec3((viewMatrix * worldMatrix * position).xyz); // position transformed to view space
    Normal = normalize((viewMatrix * worldMatrix * vec4(normal.xyz,0)).xyz);
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * position;
}
