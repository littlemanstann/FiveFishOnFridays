OF_GLSL_SHADER_HEADER

uniform vec4 globalColor;

out vec4 fragColor;
 
void main()
{
   // fragColor = vec4(1,globalColor.g,0,1);

fragColor = vec4(1, 0, 0, 1);

}
