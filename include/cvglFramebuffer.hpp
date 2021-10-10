#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "opencv2/core/utility.hpp"

#include "cvglTexture.hpp"

class cvglFramebuffer
{
public:
    cvglFramebuffer()
    {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER , m_fbo);
        // could maybe use GL_DRAW_FRAMEBUFFER since we're only writing into it?
        // was GL_FRAMEBUFFER

        texture = std::unique_ptr<cvglTexture>(new cvglTexture);

        // configure texture for framebuffer
        texture->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


        // render buffer for depth (not sure if we really need this right now but whatever)
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        // GL_DEPTH_COMPONENT instead of GL_DEPTH24_STENCIL8..?

        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getTexID(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
        // GL_DEPTH_STENCIL_ATTACHMENT vs GL_DEPTH_ATTACHMENT
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        }

        // reset to default binding
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }
    
    ~cvglFramebuffer()
    {
        glDeleteRenderbuffers(1, &m_rbo);
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    
    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

//        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glViewport( 0, 0, width, height);

        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);

    }


    inline GLuint getTexID(){ return texture->getTexID(); }
    
private:
    GLuint m_fbo = 0;
    GLuint m_rbo = 0;

    std::unique_ptr<cvglTexture> texture;

    int width = 1920;
    int height = 1080;

};
