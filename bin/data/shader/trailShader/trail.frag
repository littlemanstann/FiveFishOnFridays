#version 330
in GS_OUT {
    vec2 uv;
    float phase;
    float id;
} myfrag;

out vec4 outputColor;

uniform float t;
uniform sampler2D texIndex;

void main()
{   
    // Calculate fade based on time and phase
    float fade = 1.0-0.25*mod(t+4*myfrag.phase,4.0);

    // Find the correct cell in the 4x4 fire texture grid using the id
    float col = mod(myfrag.id, 4);
    float row = floor(myfrag.id / 4);

    // Calculate the UV coordinates within that cell using a minimum coordinate and adding the fragment UV
    vec2 minCoord = vec2(col, (4 - 1.0 - row));
    vec2 uv = minCoord + myfrag.uv;

    // Output the color from the texture and apply the fade
    outputColor = texture(texIndex, uv/4); // why divide uv by 4? Because the texture is a 4x4 grid of images!
    outputColor.a = fade;
}
