#version 330 core

uniform vec3 color;

out vec4 fragColor; //output color

void main(){
    fragColor = vec4(color,1);
}
