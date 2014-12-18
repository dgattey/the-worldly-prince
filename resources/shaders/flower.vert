#version 330 core

in vec3 position; // Position of the vertex
in vec3 normal; // normal of the vertex

uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to screen coordinates
uniform mat4 m; // model matrix (object -> world)


void main(){

    vec3 position_worldSpace = (m * vec4(position, 1.0)).xyz; // Vertex position in world space

    gl_Position =  mvp*vec4(position, 1.0); // Vertex position in screen space
}
