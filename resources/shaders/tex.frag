#version 400

in vec2 uv; // Input to this fragment shader is the output of tex.gvert

// Textures are "sampler2D" uniforms
uniform sampler2D starTex;
uniform sampler2D planetTex;

// Output color vector
out vec4 fragColor;

void main(void)
{
    vec3 planetColor = texture(planetTex, uv).rgb;

    // If flower isn't black, draw it, otherwise try planet, otherwise draw star
    if (length(planetColor) > 0) fragColor = vec4(planetColor, 1.0);
    else fragColor = vec4(texture(starTex, uv).rgb, 1.0);
}
