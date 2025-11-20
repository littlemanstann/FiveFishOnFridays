#version 150
in GS_OUT {
    vec2 uv;
    float phase;
    float id;
} myfrag;

out vec4 outputColor;
uniform sampler2D texture0;

void main()
{   

    float fade = 1.0-0.25*mod(t+4*myfrag.phase,4.0);

    vec2 uv = myfrag.uv;
/*
// commented out code creates circle
    float d = (uv.x-0.5)*(uv.x-0.5) + (uv.y-0.5)*(uv.y-0.5);

    if (d < 0.2)
       outputColor = vec4(1.0, myfrag.id, 0.2, fade);
    else
       outputColor = vec4(0,0,0,0); // nothing
*/
    outputColor = texture(texture0,uv/4); // why divide uv by 4?
    outputColor.a = fade;
}
