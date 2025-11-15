OF_GLSL_SHADER_HEADER

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
// Added specularStrength uniform
uniform float specularStrength;

out vec4 fragColor;

void main()
{
    // Ambient
    float ambientStrength = 0.1;
    float ambient = ambientStrength;

    // Diffuse
    vec3 N = normalize(Normal); // interpolation might give something that isn't unit length
    vec3 L = normalize(lightPos - FragPos);
    float diffuse = 0.5 * (1 + dot(N, L)); // Edited the ambient diffuse calcuation

    // Specular
    // Changed it to Blinn-Phong specular (uses halfwayDir)
    vec3 halfwayDir = normalize(L + normalize(viewPos - FragPos)); // Because it's in view space now, viewPos is (0,0,0)
    float spec = pow(max(dot(N, halfwayDir), 0.0), 32);
    float specular = specularStrength * spec;

    vec3 result = (ambient + diffuse + specular) * objectColor * lightColor;
    fragColor = vec4(result.rgb, 1.0);

 //   fragColor = vec4(0.7,0.3,0.1,1.0); // test with constant color
}

