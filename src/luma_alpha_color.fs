#version 330 core

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D tex;
uniform sampler2D prevTex;

uniform float scale_alpha;

uniform float prev_tex_ratio;

uniform float gamma;
uniform float contrast;
uniform float saturation;
uniform float brightness;

uniform float luma_target;
uniform float luma_tol;
uniform float luma_fade;
uniform float luma_mix;


// vignette now in screen proc

const vec4 luma_coef = vec4(0.299, 0.587, 0.114, 0.);


vec4 brightnessContrast(vec4 value, float brightness, float contrast)
{
    return vec4((value.rgb - 0.5) * contrast + 0.5 + brightness, value.a);
}

vec4 czm_saturation(vec4 value, float adjustment)
{
    // Algorithm from Chapter 16 of OpenGL Shading Language
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(value.rgb, W));
    return vec4( mix(intensity, value.rgb, adjustment), value.a);
}

vec4 gammaCorrection(vec4 value, float param)
{
    return vec4( pow( value.rgb, vec3(1. / param)), value.a);
}


vec4 lumakey(vec4 aTex, vec4 bTex)
{
    float luma = dot(aTex, luma_coef);
    float dx = abs(luma - luma_target);
    float lm_scale = smoothstep( abs(luma_tol), abs(luma_tol)+abs(luma_fade), dx);

    float mixamout = lm_scale;
    //float mixamout = mix( lm_scale, 1.0 - lm_scale, invert);
    aTex.a = lm_scale;
    return mix(bTex, aTex, vec4(mixamout));
}


void main()
{
    vec2 prevTexcoord = vec2(Texcoord.x, 1.0 - Texcoord.y);

    vec4 a = texture( tex, Texcoord );
    vec4 b = texture( prevTex, prevTexcoord);

    a = brightnessContrast(a, brightness, contrast);
    a = czm_saturation(a, saturation);
    a = gammaCorrection(a, gamma);
/*
    vec4 tex_samp = mix(a, b, prev_tex_ratio);

    float luma = dot(tex_samp, luma_coef);
    tex_samp.a = luma;
*/
    vec4 luma_proc = lumakey(a, b);
    luma_proc.a *= scale_alpha;
    a.a *= scale_alpha;

    outColor = mix(a, luma_proc, luma_mix);//lumaMix;//lumaMix;

}

