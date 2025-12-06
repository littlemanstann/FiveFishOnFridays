OF_GLSL_SHADER_HEADER


in vec3 FragPos;


out vec4 fragColor;

void main()
{


	vec3 result = vec3(min(1 - pow(abs(FragPos.z * 0.008),1.3), 1.0));

    fragColor = vec4(result.rgb, 1.0);
}

