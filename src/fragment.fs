#version 330 core

in vec2 Texcoord;
in float Time;

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
    outColor = texture( tex, Texcoord );
   /*
    outColor.r = gold_noise(Texcoord, Time*1.1);
    outColor.g = gold_noise(Texcoord, Time*1.7);
    outColor.b = gold_noise(Texcoord, Time*1.4);
    */
}
