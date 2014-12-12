#version 330

in vec2 screenCoord;
in float noise;
in float disturbance;

void main() {
    vec3 color;
    float thresh = noise * 100.0;
    
    // Lakes - creates islands too!
    if (thresh < 5.7) {
      color = vec3(0.0, 0.2, 0.5) * (0.6 + 7.0 * noise);
    }
    
    // Ground
    else {
      color = vec3(0.15, 0.4, 0.27) * (0.3 + 5.0 * noise );
    }
    // Uniform gray color, darker valleys
    // color = vec3(0.38, 0.38, 0.4) * ( 0.25 + 5.0 * noise );
    gl_FragColor = vec4( color.rgb * 1.3, 1.0 );
 
}  