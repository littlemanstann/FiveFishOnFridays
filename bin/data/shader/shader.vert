OF_GLSL_SHADER_HEADER

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform mat4 worldMatrix;

uniform float time;

in vec4 position;

void main()
{

float displacementHeight = 100;
    float displacementY = sin(time + (position.x / 100.0)) * displacementHeight;
	vec4 p = position;
	p.y += displacementY;
    vec4 modifiedPosition = modelViewProjectionMatrix *worldMatrix* p;

	gl_Position = modifiedPosition;
}
