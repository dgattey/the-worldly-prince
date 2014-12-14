#version 330

in vec2 uv; // uv coordinate for frag position

uniform mat4 mvp; // Modelview Projection matrix. This maps the vertices in model (object) space to film coordinates (world->film)
uniform vec4 color;
uniform sampler2D textureSampler;

out vec4 fragColor; //output color

void main(){
    // Figure out something reasonable to do with texColor and color...
    vec4 texColor = texture(textureSampler, uv);
    float dx = 0.5f - uv.x;
    float dy = 0.5f - uv.y;
    float radius = sqrt(dx*dx + dy*dy);
    float opacity = pow(0.75f - radius,2);
    fragColor = color * color.a * opacity * 3.0f;//texColor*color;//(0.5*color) + (0.5*texColor);
}
