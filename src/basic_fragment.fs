#version 330 core

in vec2 Texcoord;

uniform sampler2D framebuffer_tex;

out vec4 screen_outColor;

void main()
{

    vec2 prevTexcoord = vec2(Texcoord.x, 1.0 - Texcoord.y);

    vec4 tex_samp = texture( framebuffer_tex, prevTexcoord );

    screen_outColor = tex_samp;

}
