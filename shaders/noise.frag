#version 330

in vec2 screenCoord;
in float noise;

void main() {
    vec3 color;
    float thresh = noise * 100.0;
    
    // Lakes - creates islands too!
    if (thresh < 5.7) {
      color = vec3(0.0, 0.2, 0.5) * (0.55 + 6.5 * noise);
    }
    
    // Ground
    else {
      color = vec3(0.05, 0.5, 0.21) * (0.06 + 5.5 * noise );
    }
    // Uniform gray color, darker valleys
    // color = vec3(0.38, 0.38, 0.4) * ( 0.25 + 5.0 * noise );
    gl_FragColor = vec4( color.rgb * 1.2, 1.0 );
 
}  