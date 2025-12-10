#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

//uniform mat4 modelViewProjectionMatrix;
uniform float pSize;


in VS_OUT {
    float phase;
    float id;
} gs_in[]; // array, even though with "points" specified, the array is length one

out GS_OUT {
    vec2 uv;
    float phase;
    float id;
} gs_out;


void main() {
    vec4 center = gl_in[0].gl_Position;

    // Create quad around center
    vec2 offsets[4] = vec2[](
        vec2(-pSize, -pSize),
        vec2(pSize, -pSize),
        vec2(-pSize, pSize),
        vec2(pSize, pSize)
    );

    vec2 uv_set[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(0.0, 1.0),
        vec2(1.0, 1.0)
    );

    for (int i = 0; i < 4; i++) {
        vec4 offset = vec4(offsets[i], 0.0, 0.0);
        gs_out.phase = gs_in[0].phase;
	gs_out.id = gs_in[0].id;
        gs_out.uv = uv_set[i];
        gl_Position = center + offset;
        EmitVertex();
    }
    EndPrimitive();
}
