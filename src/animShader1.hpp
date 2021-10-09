#pragma once

#include "cvglShaderFramebuffer.hpp"


struct animShader1 : public cvglShaderFramebuffer
{
//public:


    // public params, can be set directly
    // updated in gl thread in render()

    float contrast = 1, brightness = 0, saturation = 1, gamma = 1;

    float luma_target = 0.1;
    float luma_tol = 0.1;
    float luma_fade = 0.48;
    float luma_mix = 0.75;

    glm::vec4 vignette_xyr_aspect;
    glm::mat4 transform_matrix;

    animShader1()
    {
        std::string path = "/home/rama/animism-cvgl/src/";
        if( !loadShaderFiles( path + "vertex.vs", path + "fragment.fs" ) ){
            printf("failed to load base shader\n");
            // probably throw an error here
        }

        init();
        use();

        //
        setInt("tex", 0);
        setInt("prevTex", 1);

    }


    void updateParams()
    {
        setFloat("gamma", gamma);
        setFloat("contrast", contrast);
        setFloat("saturation", saturation);
        setFloat("brightness", brightness);
        setFloat("scale_alpha", 1);

        setFloat("luma_target", luma_target);
        setFloat("luma_tol", luma_tol);
        setFloat("luma_fade", luma_fade);
        setFloat("luma_mix", luma_mix);

        setMat4("transform_matrix", transform_matrix );
        setVec4("vignette_xyr_aspect", vignette_xyr_aspect);
    }

    // must be bound first via bind()
    GLuint render( std::vector<GLuint> texIDs ) override
    {
        cvglShaderFramebuffer::render(texIDs);
        updateParams();


        return getTexID();
    }


};

