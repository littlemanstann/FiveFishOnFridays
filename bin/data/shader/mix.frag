#version 150

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D friedchicken;
uniform vec3 waterColor;

out vec4 fragColor;



void main() 
{
	vec4 color;
	vec2 pos = uv0;
	color = texture(tex0,uv0);

	vec4 depth;
	vec2 pos2 = uv0;
	depth = texture(tex1,uv0);

	vec3 lerpColour = vec3( waterColor * (1 - depth.r) + color.rgb * (depth.r));

	vec3 col = lerpColour;

  	fragColor = vec4(col, color.a);
}