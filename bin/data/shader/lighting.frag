OF_GLSL_SHADER_HEADER

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D tex0;
uniform int texBool;
uniform int brightBool;
uniform vec2 resolution;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor; // not presently used, mix in if you like
uniform vec3 objectColor;
uniform float specularPower;

out vec4 fragColor;

void main()
{

    // Ambient
    float ambientStrength = 0.1;
    float ambient = ambientStrength;

    // Diffuse
    vec3 N = normalize(Normal); // interpolation might give something that isn't unit length
    vec3 L = normalize(lightPos - FragPos);
    float diffuse = 0.5*(1.0 + dot(N,L)); // nonphysical ambient-diffuse

    // Specular
	// Not using R anymore so commented out
	// Calculate H for Blinn-Phong, then do N dot H for spec
    float specularStrength = 1.5;
    vec3 V = normalize(viewPos - FragPos);
	vec3 H = normalize(V+L);
    //vec3 R = normalize(-L + 2.0*N*dot(L,N));
    float spec = pow(max(dot(N, H), 0.0), specularPower);
    float specular = specularStrength * spec;

	vec3 result = vec3(0.,0.,0.);


    if(texBool == 1){
        result = max((ambient + diffuse + specular), brightBool) * texture(tex0, TexCoord).rgb;
    }else{
        result = max((ambient + diffuse + specular), brightBool) * objectColor;
    }
	result = min(result, 1.0);
    fragColor = vec4(result.rgb, 1.0);
/*
if (FragPos.x > 0) fragColor = vec4(1,0,0,1);
    fragColor.rgb = vec3(N.x*0.5+0.5, 0.3, N.z*0.5+0.5);
*/
 //   fragColor = vec4(0.7,0.3,0.1,1.0); // test with constant color
}
