#version 330 core

in vec2 Texcoord;
out vec4 outColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform float scale_alpha;

uniform vec2 hsflow_scale;
uniform vec2 hsflow_offset;
uniform float hsflow_lambda;

uniform vec2 repos_amt;
uniform vec4 repos_scale;
uniform vec4 repos_bias;
//uniform vec2 repos_boundmode;

uniform float flow_mix;

uniform float slide_down;
uniform float slide_up;

uniform vec4 vignette_xyr_aspect;
uniform float time;

const vec4 luma_coef = vec4(0.299, 0.587, 0.114, 0.);


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

const float PHI = 1.61803398874989484820459; // Φ = Golden Ratio

float gold_noise(vec2 xy, float seed)
{
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

vec4 tp_slide(vec4 input0, vec4 input1)
{
    vec4 su, sd, up, down, amount;

    amount.x = input0.x > input1.x ? 1.0 : 0.0;
    amount.y = input0.y > input1.y ? 1.0 : 0.0;
    amount.z = input0.z > input1.z ? 1.0 : 0.0;
    amount.w = input0.w > input1.w ? 1.0 : 0.0;

    float d = max(1.0, abs(slide_down));
    sd = vec4( 1.0 / d );
    down = input1 + ((input0 - input1) * sd);

    float u = max(1.0, abs(slide_up));
    su = vec4( 1.0 / u );
    up = input1 + ((input0 - input1) * su);

    return mix(down, up, amount);
}

vec4 ab_hsflow_repos(sampler2D tex0, sampler2D tex1, vec2 texcoord0, vec2 texcoord1)
{
/*
        vec4 a = texture2DRect(tex0, texcoord0);
        vec4 b = texture2DRect(tex1, texcoord1);
        vec2 x1 = vec2(offset.x,0.);
        vec2 y1 = vec2(0.,offset.y);

        //get the difference
        vec4 curdif = b-a;

        //calculate the gradient
        vec4 gradx = texture2DRect(tex1, texcoord1+x1)-texture2DRect(tex1, texcoord1-x1);
        gradx += texture2DRect(tex0, texcoord0+x1)-texture2DRect(tex0, texcoord0-x1);
        vec4 grady = texture2DRect(tex1, texcoord1+y1)-texture2DRect(tex1, texcoord1-y1);
        grady += texture2DRect(tex0, texcoord0+y1)-texture2DRect(tex0, texcoord0-y1);
        vec4 gradmag = sqrt((gradx*gradx)+(grady*grady)+vec4(lambda));

        vec4 vx = curdif*(gradx/gradmag);
        float vxd = vx.r;//assumes greyscale
        //format output for flowrepos, out(-x,+x,-y,+y)
        vec2 xout = vec2(max(vxd,0.),abs(min(vxd,0.)))*scale.x;

        vec4 vy = curdif*(grady/gradmag);
        float vyd = vy.r;//assumes greyscale
        //format output for flowrepos, out(-x,+x,-y,+y)
        vec2 yout = vec2(max(vyd,0.),abs(min(vyd,0.)))*scale.y;
    */
    vec4 a = texture( tex0, texcoord0 );
    vec4 b = texture( tex1, texcoord1 );

    vec2 x1 = vec2( hsflow_offset.x, 0.);
    vec2 y1 = vec2( 0., hsflow_offset.y);

    vec4 curdif = a - b;

    //calculate the gradient
    vec4 gradx, grady;

    gradx =  texture(tex1, texcoord1+x1)-texture(tex1, texcoord1-x1);
    gradx += texture(tex0, texcoord0+x1)-texture(tex0, texcoord0-x1);

    grady =  texture(tex1, texcoord1+y1)-texture(tex1, texcoord1-y1);
    grady += texture(tex0, texcoord0+y1)-texture(tex0, texcoord0-y1);

    vec4 gradmag = sqrt( (gradx*gradx) + (grady*grady) + vec4(hsflow_lambda) );

    vec4 vx = curdif*(gradx/gradmag);

// editing here, trying to figure out why it's not moving like ab's does
    float luma = dot(vx, luma_coef);

    float vxd = luma;//vx.r;//assumes greyscale

    //format output for flowrepos, out(-x,+x,-y,+y)
    vec2 xout = vec2(max(vxd,0.),abs(min(vxd,0.))) * hsflow_scale.x;

    vec4 vy = curdif*(grady/gradmag);
    float vyd = vy.r;//assumes greyscale
    //format output for flowrepos, out(-x,+x,-y,+y)
    vec2 yout = vec2(max(vyd,0.),abs(min(vyd,0.))) * hsflow_scale.y;

    //return vec4(xout.xy,yout.xy);

    vec4 look = vec4(xout.xy,yout.xy);

// repos
    //vec4 look = texture2DRect(tex1,texcoord1);//sample repos texture
    vec2 offs = vec2(look.y-look.x,look.w-look.z) * repos_amt;
    vec2 coord = offs + texcoord0;//relative coordinates
   //coord = mod(coord,texdim0);
    vec4 repos = texture(tex0, coord);

   // output texture
    return repos * repos_scale + repos_bias;

}


void main()
{
    vec2 invCoord = vec2(Texcoord.x, 1.0 - Texcoord.y);

    vec2 texcoord0 = Texcoord;
    vec2 texcoord1 = invCoord;

    vec4 a = texture( tex0, texcoord0 );
    vec4 b = texture( tex1, texcoord1 );

    //a.a *= scale_alpha;


    // ab_hsflow
    vec2 x1 = vec2( hsflow_offset.x, 0.);
    vec2 y1 = vec2( 0., hsflow_offset.y);

    vec4 curdif = a - b;

    //calculate the gradient
    vec4 gradx, grady;

    gradx =  texture(tex1, texcoord1+x1)-texture(tex1, texcoord1-x1);
    gradx += texture(tex0, texcoord0+x1)-texture(tex0, texcoord0-x1);

    grady =  texture(tex1, texcoord1+y1)-texture(tex1, texcoord1-y1);
    grady += texture(tex0, texcoord0+y1)-texture(tex0, texcoord0-y1);

    vec4 gradmag = sqrt( (gradx*gradx) + (grady*grady) + vec4(hsflow_lambda) );

    vec4 vx = curdif * (gradx/gradmag);

    float xluma = dot(vx, luma_coef);
    float vxd = xluma; //vx.r;//assumes greyscale
    //format output for flowrepos, out(-x,+x,-y,+y)
    vec2 xout = vec2( max(vxd,0.), abs(min(vxd,0.)) ) * hsflow_scale.x;

    vec4 vy = curdif*(grady/gradmag);

    float yluma = dot(vy, luma_coef);
    float vyd = yluma;// vy.r;//assumes greyscale

//format output for flowrepos, out(-x,+x,-y,+y)
    vec2 yout = vec2( max(vyd,0.), abs(min(vyd,0.)) ) * hsflow_scale.y;

    //return vec4(xout.xy,yout.xy);

    vec4 look = vec4(xout.xy, yout.xy);

    // repos
    //vec4 look = texture2DRect(tex1,texcoord1);//sample repos texture
    vec2 offs = vec2(look.y-look.x, look.w-look.z) * repos_amt;
    vec2 coord = offs + texcoord1;//relative coordinates
    //coord = mod(coord,texdim0);
    vec4 repos = texture(tex1, coord);

    // output texture
    vec4 hsflow =  repos * repos_scale + repos_bias;

    outColor = mix(a, hsflow, flow_mix);//mix(a, hsflow, 0.75); ;//hsflow;//hsflow;/// mix(a,b, 0.5);//mix(a, hsflow, 0.75);

    /*
    if( vignette_xyr_aspect.z == 1 )
    {
        outColor = tex_samp;
    }
    else
    {
        float xfade = 0.05;
        float dx = vignette_xyr_aspect.x - Texcoord.x;
        float dy = vignette_xyr_aspect.y - (Texcoord.y / vignette_xyr_aspect.w);
        float dist = sqrt(dx*dx + dy*dy);

        if( dist < vignette_xyr_aspect.z )
        {
            outColor = tex_samp;
        }
        else if( (dist - vignette_xyr_aspect.z) <= xfade )
        {
            float fade = (dist - vignette_xyr_aspect.z) / xfade;

            outColor = tex_samp * vec4(1, 1, 1, 1 - fade * fade * fade);

        }
        else
        {
            outColor = vec4(0,0,0,1);
        }
    }
    */

}

