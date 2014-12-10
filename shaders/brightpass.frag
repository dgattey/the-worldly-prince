#version 400

uniform sampler2D tex;

in vec2 uv;

out vec4 fragColor;

const vec3 avgVector = vec3(0.299, 0.587, 0.114);


void main(void)
{
    vec4 color = texture( tex, uv.xy );
    float luminance = max( 0.0, dot( avgVector, color.rgb ) );

    color = vec4(vec3(luminance),1); // 256 shades of grey. Way too many.

    // TODO - Step 2.3: Set color to either 1 or 0 based on luminance
    if (luminance < 1.0)
        fragColor = vec4(0);
    else fragColor = color;
}

