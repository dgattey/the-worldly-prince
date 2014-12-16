#version 330 core

uniform vec3 color;

out vec4 fragColor;

void main()
{
    fragColor = vec4(clamp(color * 0.75, 0.0, 1.0), 1.0);

}
