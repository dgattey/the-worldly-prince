#version 330 core

in vec3 position; // Position of the vertex
in vec3 normal; // normal of the vertex

uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to screen coordinates

void main(){

    gl_Position =  mvp*vec4(position, 1.0);

}
