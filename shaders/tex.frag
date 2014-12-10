#version 400

in vec2 uv; // Input to this fragment shader is the output of tex.gvert

// Textures are "sampler2D" uniforms
uniform sampler2D originalTex;
uniform sampler2D blurTex;

// Output color vector
out vec4 fragColor;

void main(void)
{
    fragColor = vec4( texture( originalTex, uv ).rgb + texture( blurTex, uv ).rgb, 1.0 );
}
