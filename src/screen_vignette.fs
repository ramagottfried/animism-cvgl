#version 330 core

in vec2 Texcoord;

uniform sampler2D framebuffer_tex;
uniform vec4 vignette_xyr_aspect;

out vec4 screen_outColor;

void main()
{

    vec2 prevTexcoord = vec2(Texcoord.x, 1.0 - Texcoord.y);

    vec4 tex_samp = texture( framebuffer_tex, prevTexcoord );


    if( vignette_xyr_aspect.z == 1 )
    {
        screen_outColor = tex_samp;
    }
    else
    {
        float xfade = 0.05;
        float dx = vignette_xyr_aspect.x - Texcoord.x;
        float dy = vignette_xyr_aspect.y - (Texcoord.y / vignette_xyr_aspect.w);
        float dist = sqrt(dx*dx + dy*dy);

        if( dist < vignette_xyr_aspect.z )
        {
            screen_outColor = tex_samp;
        }
        else if( (dist - vignette_xyr_aspect.z) <= xfade )
        {
            float fade = (dist - vignette_xyr_aspect.z) / xfade;

            screen_outColor = tex_samp * vec4(1, 1, 1, 1 - fade * fade * fade);

        }
        else
        {
            screen_outColor = vec4(0,0,0,1);
        }
    }


    //screen_outColor = tex_samp;

}
