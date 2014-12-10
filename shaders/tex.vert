#version 400

in vec3 position; // Position of the vertex
in vec2 texCoords; // texture coordinate of the vertex

out vec2 uv; // Shader output that goes to fragment shader

void main(void)
{
    uv = texCoords;
    gl_Position = vec4(position.xyz, 1.0);
}
