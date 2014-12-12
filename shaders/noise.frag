#version 330

in float noise;

void main() {
    vec3 color;
    float thresh = noise * 100.0;
    
    // Lakes - creates islands too!
    }
    
    // Ground
    else {
      color = vec3(0.05, 0.5, 0.21) * (0.075 + 10.0 * noise );
    }
    // Uniform gray color, darker valleys
    //color = vec3(0.38, 0.38, 0.4) * ( 0.25 + 5.0 * noise );
    gl_FragColor = vec4( color.rgb * 1.3, 1.0 );
 
}  
