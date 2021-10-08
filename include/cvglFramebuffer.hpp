#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "opencv2/core/utility.hpp"

class cvglFramebuffer
{
public:
    cvglFramebuffer()
    {
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER , m_fbo);
        // coudl maybe use GL_DRAW_FRAMEBUFFER since we're only writing into it?
        // was GL_FRAMEBUFFER

        glGenTextures(1, &m_tex);
        glBindTexture(GL_TEXTURE_2D, m_tex);

        // setting dummy texture content for setup
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        // wrap parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // or GL_CLAMP_TO_EDGE?

        // interpolation
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // try GL_NEAREST also
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        // render buffer for depth (not sure if we really need this right now but whatever)
        glGenRenderbuffers(1, &m_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1920, 1080);
        // GL_DEPTH_COMPONENT instead of GL_DEPTH24_STENCIL8..?

        // attach texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
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
        glDeleteTextures(1, &m_tex);
        glDeleteFramebuffers(1, &m_fbo);
        m_tex = 0;
        m_fbo = 0;
    }
    
    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }


    inline GLuint getTexID(){ return m_tex; }
    
private:
    GLuint m_fbo = 0;
    GLuint m_tex = 0;
    GLuint m_rbo = 0;

};
