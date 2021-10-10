#version 330 core

in vec2 Texcoord;
out vec4 screen_outColor;

uniform sampler2D framebuffer_tex;
uniform vec4 vignette_xyr_aspect;
uniform float vignette_fadeSize;

uniform float time;

uniform float noise_mult;
uniform float noise_mix;

highp float rand(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

void main()
{

    vec2 texcord0 = vec2(Texcoord.x, 1.0 - Texcoord.y);
    vec4 tex_samp = texture( framebuffer_tex, texcord0 );

    vec4 bw_noise = vec4( vec3( rand(texcord0 + fract(time)) ), 1.0);
    vec4 bw_noise_mult = mix(bw_noise, bw_noise * tex_samp, noise_mult);

    vec4 outSamp = mix(tex_samp, bw_noise_mult, noise_mix);


    if( vignette_xyr_aspect.z != 1 )
    {
       // float xfade = 0.05;
        float rad = vignette_xyr_aspect.z;
        float aspect = vignette_xyr_aspect.w;

        float dx = vignette_xyr_aspect.x - Texcoord.x;
        float dy = vignette_xyr_aspect.y - (Texcoord.y / aspect);

        float dist = sqrt(dx*dx + dy*dy);

        if( dist < rad )
        {
            ;
        }
        else if( (dist - rad) <= vignette_fadeSize )
        {
            float fade = 1 - (dist - rad) / vignette_fadeSize;

            outSamp *= fade*fade*fade;//vec4(1, 1, 1, 1 - fade * fade * fade);

        }
        else
        {
            outSamp = vec4(0,0,0,1);
        }
    }


    screen_outColor = outSamp;


}




/*
vec4 color_noise = vec4(    rand(Texcoord + fract(time) * floor(time + 1) ),
                            rand(Texcoord + fract(time) * floor(time + 2) ),
                            rand(Texcoord + fract(time) * floor(time + 3) ),
                            1.0);
*/

