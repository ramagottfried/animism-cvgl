#version 330 core

in vec2 Texcoord;

uniform vec4 vignette_xyr_aspect;

uniform float time;

uniform sampler2D tex;
out vec4 outColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

const float PHI = 1.61803398874989484820459; // Φ = Golden Ratio

float gold_noise(vec2 xy, float seed)
{
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

void main()
{

    if( vignette_xyr_aspect.z == 1 )
    {
        outColor = texture( tex, Texcoord );
    }
    else
    {
        float xfade = 0.05;
        float dx = vignette_xyr_aspect.x - Texcoord.x;
        float dy = vignette_xyr_aspect.y - (Texcoord.y / vignette_xyr_aspect.w);
        float dist = sqrt(dx*dx + dy*dy);

        if( dist < vignette_xyr_aspect.z )
        {
            outColor = texture( tex, Texcoord );
        }
        else if( (dist - vignette_xyr_aspect.z) <= xfade )
        {
            float fade = (dist - vignette_xyr_aspect.z) / xfade;

            outColor = texture( tex, Texcoord ) * vec4(1, 1, 1, 1 - fade * fade * fade);

        }
        else
        {
            outColor = vec4(0,0,0,1);
        }
    }


   /*
    outColor.r = gold_noise(Texcoord, Time*1.1);
    outColor.g = gold_noise(Texcoord, Time*1.7);
    outColor.b = gold_noise(Texcoord, Time*1.4);
    */
}
