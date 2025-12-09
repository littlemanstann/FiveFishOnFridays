#version 150
// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform float dashIntensity; // 0.0 to 1.0, animate this when dashing
uniform vec3 waterColor;
out vec4 fragColor;

// Simple hash function for pseudo-random values per streak
float hash(float n)
{
    return fract(sin(n) * 43758.5453123);
}

void main() 
{
    vec2 center = vec2(0.5, 0.5);
    vec2 dir = uv0 - center;
    float dist = length(dir);
    
    
    // Zoom effect - sample slightly toward center
    float zoomAmount = dashIntensity * 0.05;
    vec2 zoomedUV = center + dir * (1.0 - zoomAmount);
    
    // Radial blur parameters
    int samples = 12;
    float blurStrength = dashIntensity * 0.02;
    
    vec4 color = vec4(0.0);
    
    // Sample along the radial direction
    for(int i = 0; i < samples; i++)
    {
        float offset = blurStrength * float(i) / float(samples - 1);
        vec2 sampleUV = zoomedUV + dir * offset;
        color += texture(tex0, sampleUV);
    }
    
    color /= float(samples);
    
    // Add white gradient glow (stronger toward edges)
    float edgeFactor = smoothstep(0.0, 0.6, dist);
    float whiteness = dashIntensity * edgeFactor * 0.3;
    
    // Discrete radial streaks with independent flickering
    float angle = atan(dir.y, dir.x);
    int numStreaks = 32; // More streaks
    float streakWidth = 0.04; // Thinner streaks
    
    float flickerSpeed = 8.0; // Fast flicker rate
    
    float streakIntensity = 0.0;
    for(int i = 0; i < numStreaks; i++)
    {
        float streakAngle = (float(i) / float(numStreaks)) * 6.28318;
        streakAngle += hash(float(i)) * 2.0; // Random offset per streak
        
        float angleDiff = abs(mod(angle - streakAngle + 3.14159, 6.28318) - 3.14159);
        
        // Create thin angular wedge
        float streakThickness = streakWidth;
        float streak = 1.0 - smoothstep(0.0, streakThickness, angleDiff);
        
        // Independent flicker for each streak
        float flickerPhase = timer * flickerSpeed + hash(float(i) * 10.0) * 100.0;
        float flicker = step(0.5, fract(flickerPhase)); // On/off flicker
        
        // Add some variation to flicker timing per streak
        float flickerRate = 0.3 + hash(float(i) * 20.0) * 0.7; // Each has different rate
        flicker *= step(flickerRate, fract(timer * flickerSpeed * 0.5 + hash(float(i) * 30.0) * 10.0));
        
        // Only show streaks away from center
        float radialFade = smoothstep(0.15, 0.35, dist) * (1.0 - smoothstep(0.65, 0.9, dist));
        
        streakIntensity += streak * flicker * radialFade;
    }
    
    streakIntensity = clamp(streakIntensity, 0.0, 1.0);
    whiteness += streakIntensity * dashIntensity * 0.8;
    
    color.rgb += vec3(whiteness);
    //Speed Shader End


    //Depth Shader Begin
	vec4 depth;
	depth = texture(tex1,uv0);

	color.rgb = vec3( waterColor * (1 - depth.r) + color.rgb * (depth.r));
    //Depth Shader End
    
    //Output the Color
    fragColor = color;
    
}
