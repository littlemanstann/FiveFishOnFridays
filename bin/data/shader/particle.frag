#version 150

in vec2 uv;

out vec4 outputColor;

void main()
{   
    float d = (uv.x-0.5)*(uv.x-0.5) + (uv.y-0.5)*(uv.y-0.5);

    if (d < 0.2)
       outputColor = vec4(1.0, 0.5, 0.2, 1.0); // orange-ish
    else
       outputColor = vec4(0,0,0,0); // nothing
}
