#version 400

const int MAX_KERNEL_SIZE = 64;
uniform sampler2D tex;
uniform int arraySize;
uniform vec2 offsets[MAX_KERNEL_SIZE];
uniform float kernel[MAX_KERNEL_SIZE];

in vec2 uv;

out vec4 fragColor;

void main(void)
{
    vec4 color = vec4( 0.0 );
//    color = texture(tex, uv);

//    vec4 color = texture( tex, uv.xy );
    for (int i=0; i<arraySize; i++) {
//        vec2 offset = offsets[i];
        color += texture(tex, uv.xy+offsets[i].xy) * kernel[i];

    }

    // TODO: Step 2.4 - Fill this in!

    fragColor = color;
}
