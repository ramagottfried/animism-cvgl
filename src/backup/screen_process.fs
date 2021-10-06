#version 330 core

in vec2 Texcoord;

uniform sampler2D framebuffer_tex;

out vec4 screen_outColor;

void main()
{

    vec4 tex_samp = texture( framebuffer_tex, Texcoord );

    screen_outColor = tex_samp;

}
