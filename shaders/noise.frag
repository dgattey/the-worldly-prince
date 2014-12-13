#version 330 core

in float noise;

out vec4 fragColor;

void main() {
    float thresh = noise * 100.0;
    
    vec3 color = vec3(0.38, 0.38, 0.4) * ( 0.25 + 5.0 * noise );
    fragColor = vec4( color.rgb * 1.3, 1.0 );
 
}  
