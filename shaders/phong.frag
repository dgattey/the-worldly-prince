#version 330 core

const int numLights = 4;
uniform float k_a, k_d;
uniform vec3 O_a, O_d, i_a;
uniform vec3 lightIntensities[numLights];
uniform vec3 lightPositions[numLights];

in vec3 normal_worldspace;
in vec3 lightDirs[numLights];

out vec4 fragColor; // Output color

void main()
{
    fragColor = vec4(0.0); // Output the color to this variable

    // TODO - Step 1.5: Compute the phong lighting model
    vec3 color = i_a * k_a * O_a;
    
    for (int i=0; i<numLights; i++) {
        color += lightIntensities[i]*k_d*O_d*(dot(normal_worldspace, lightDirs[i]));
    }

    fragColor = vec4(clamp(color, 0.0, 1.0), 1.0);

}
