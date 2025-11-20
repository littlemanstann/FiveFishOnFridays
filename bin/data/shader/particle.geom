#version 150

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float pSize;

out vec2 uv;

void main() {
    vec4 center = gl_in[0].gl_Position;

    // Create quad around center
    vec2 offsets[4] = vec2[](
        vec2(-pSize, -pSize),
        vec2(pSize, -pSize),
        vec2(-pSize, pSize),
        vec2(pSize, pSize)
    );

    // texture coordinates for each of the new vertices
    vec2 uv_set[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    // now actually create them using the above data
    for (int i = 0; i < 4; i++) {
        vec4 offset = vec4(offsets[i], -0.1, 0.0);
	uv = uv_set[i];
        gl_Position = center + offset;
        EmitVertex();
    }
    EndPrimitive();
}

