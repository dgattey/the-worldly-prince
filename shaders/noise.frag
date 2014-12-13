#version 330 core

in float noise;

uniform float threshold;
uniform vec4 colorLow; // 4th parameter is mix variable
uniform vec4 colorHigh; // 4th parameter is mix variable

out vec4 fragColor;

void main() {
    vec4 color;
    float height = noise * 100.0;
    
    // Low color
    if (height < threshold) color = colorLow;
    
    // Ground
    else color = colorHigh;
    fragColor = vec4(color.xyz * (color.w + 10.0*noise), 1.0);
 
}  
