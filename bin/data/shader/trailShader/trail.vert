#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in float phase;
layout(location = 3) in float id;

out VS_OUT {
    float phase;
    float id;
} vs_out;

uniform float t;
uniform mat4 MVP;

void main() {

    // Animate position using normal and time

    float g = 2; // slow motion

	float speed = 0.5;

    vec3 zeroposition = vec3(0,0,0);
    float ext = mod(t+4*phase, 4.0); // explode again every 4 seconds
    vec3 animatedPos = zeroposition + speed * 4 * normal * ext;

    // Add LINEAR forward motion
    animatedPos.z -= speed * ext * 4;

    // Add corkscrew motion using sine and cosine functions, using normal.z as angle offset
    float screwRadius = 0.3 + 0.2 * length(normal);
    animatedPos.y += (screwRadius * ext) * sin(normal.z + t * 5.0 + phase * 10.0);
    animatedPos.x += (screwRadius * ext) * cos(normal.z + t * 5.0 + phase * 10.0);

    vec4 a_final = MVP*vec4(animatedPos,1.0); // transformed animated position
	
	vs_out.phase = phase;
	vs_out.id = id; // passing through vertex phase, id to later in the pipeline
	gl_Position = a_final;

}