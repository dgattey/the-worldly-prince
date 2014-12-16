#version 330 core

in vec3 position; // Position of the vertex
in vec2 texCoord; // UV texture coordinates of the vertex

out vec2 uv; //UV texture coordinates of the vertex

uniform mat4 m; // model matrix (object -> world)
uniform mat4 v; // viewing matrix (world -> camera)
uniform mat4 mvp; // Modelview Projection matrix. (object -> film)

void main(){
        uv = texCoord;

        gl_Position =  mvp*vec4(position, 1.0);
}
