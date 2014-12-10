#version 330 core

in vec3 position; // Position of the vertex
in vec3 normal; // normal of the vertex

const int numLights = 4;
uniform vec3 lightPositions[numLights];
uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to screen coordinates
uniform mat4 m; // model matrix (object -> world)

out vec3 lightDirs[numLights];
out vec3 normal_worldspace;

void main(){

    vec3 position_worldSpace = (m * vec4(position, 1.0)).xyz; // Vertex position in world space
    normal_worldspace = normalize(mat3(transpose(inverse(m))) * normal); // Normal vector in world space

    gl_Position =  mvp*vec4(position, 1.0); // Vertex position in screen space

    // TODO - Step 1.2: Fill in lightDirs array with world-space vectors from the vertex to each light
    for (int i=0; i<numLights; i++) {
        lightDirs[i] = normalize(lightPositions[i] - position_worldSpace);
    }

}
