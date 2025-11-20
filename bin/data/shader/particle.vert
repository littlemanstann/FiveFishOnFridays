#version 150

in vec3 position;
in vec3 normal;
uniform float speed;
uniform float t;
uniform mat4 MVP;

void main() {

/*
    vec4 temp_Position = MVP*vec4(position,1.0);

//temp_Position.z = clamp(temp_Position.z, -temp_Position.w, temp_Position.w);
gl_Position = temp_Position;

*/


    // Animate position using normal and time

    float g = 0.1; // slow motion

	// float speed = 0.05;

    vec3 zeroposition = vec3(0,0,0);
    float ext = mod(t, 4.0); // explode again every 4 seconds
    vec3 animatedPos = zeroposition + speed* 4*normal * ext;
    animatedPos.y -= speed+g*ext*ext; // falling under gravity, dist = 1/2 a t ^2
    animatedPos.y += speed+g*ext; // start with upwards velocity

    vec4 a_final = MVP*vec4(animatedPos,1.0); // transformed animated position

	gl_Position = a_final;

}