#pragma once

#include "cvglShader.hpp"
#include "cvglFramebuffer.hpp"
#include "cvglObject.hpp"

/**
 * getting there,
 * but not quite fully there yet
 * some complications:
 *  how to draw differnt shapes inside here? that would mean that all
 *  gl objects also need to be in this class... maybe that makes sense? not sure yet
 *
 *  also each shaderFramebuffer should have it's own rect to draw... how to
 *  override that?
 *          maybe startRender(), and endRender() functions to handle that?
 *
 *          hhmmm no, the texture id is created by this shader,
 *          but another framebuffer is used to draw this texture...
 */

class cvglShaderFramebuffer : public cvglShader
{
public:

    cvglShaderFramebuffer(){}
    cvglShaderFramebuffer(const char * vertex_file_path, const char * fragment_file_path)
    {
        loadShaderFiles(vertex_file_path, fragment_file_path);
    }

    void init()
    {
        fbo[0] = std::unique_ptr<cvglFramebuffer>(new cvglFramebuffer);
        fbo[1] = std::unique_ptr<cvglFramebuffer>(new cvglFramebuffer);
    }

    void bind()
    {
        prevIDX = idx;
        idx = (idx+1) % 2;

        fbo[idx]->bind();

        //enable this shader
        use();

    }

    GLuint getPrevTexID()
    {
        return fbo[prevIDX]->getTexID();
    }

    GLuint getTexID()
    {
        return fbo[idx]->getTexID();
    }

    void setActiveTextures( std::vector<GLuint> texIDs )
    {
        for( GLuint i = 0; i < texIDs.size(); i++ )
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texIDs[i] );

        }
    }

    /**
     * @brief render
     * @param texIDs -- vector of texture ids to be used by shader
     * @return id of this rendered FBO texture (can be stored and used until next write?)
     */
    virtual GLuint render( std::vector<GLuint> texIDs )
    {
        // caller should call bind() first before calling render

        setActiveTextures(texIDs);

        // do processing here

        return getTexID();
    }


    /**
    * idea here is to make a double buffering framebuffer that processes
    *
    */

private:

    int idx = 0, prevIDX = 0;
    std::unique_ptr<cvglFramebuffer> fbo[2];

};
