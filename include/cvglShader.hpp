#pragma once

// Include standard headers
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>

//#include "opencv2/core/utility.hpp"

class cvglShader
{
public:
    cvglShader(){}

    cvglShader(GLuint id) : m_shaderProgram(id), freeShader(false) {}

    cvglShader(const char * vertex_file_path, const char * fragment_file_path)
    {
        loadShaderFiles(vertex_file_path, fragment_file_path);
    }

    ~cvglShader()
    {
        if( m_shaderProgram && freeShader )
            glDeleteProgram(m_shaderProgram);
    }


    int loadShaderFiles(const char * vertex_file_path, const char * fragment_file_path);
    int loadShaderFiles(const std::string& vertex_file_path, const std::string& fragment_file_path);

    int loadShaders(const GLchar* vertex_src, const GLchar* fragment_src);

    inline GLuint use()
    {
        glUseProgram(m_shaderProgram);
/*
        for( auto& u : uniforms )
        {
            printf("%u %s %u %u\n", m_shaderProgram, u.first.c_str(), u.second, glGetUniformLocation(m_shaderProgram,  u.first.c_str()) );
        }
*/
        return m_shaderProgram;
    }

    void setBool(const std::string &name, bool value)
    {
        glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), (int)value);
    }

    void setInt(const std::string &name, int value)
    {
        glUniform1i(glGetUniformLocation(m_shaderProgram, name.c_str()), value);
    }

    void setFloat(const std::string &name, float value)
    {
  //     printf("setting %s value %f\n", name.c_str(), value);
        glUniform1f(glGetUniformLocation(m_shaderProgram, name.c_str()) , value);
    }

    void setVec2(const std::string &name, const glm::vec2 &value)
    {
        glUniform2fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y)
    {
        glUniform2f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y);
    }

    void setVec3(const std::string &name, const glm::vec3 &value)
    {
        glUniform3fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y, z);
    }

    void setVec4(const std::string &name, const glm::vec4 &value)
    {
        glUniform4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
     //   printf("setting %s\n", name.c_str());
        glUniform4f(glGetUniformLocation(m_shaderProgram, name.c_str()), x, y, z, w);
    }

    void setMat2(const std::string &name, const glm::mat2 &mat)
    {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string &name, const glm::mat3 &mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat)
    {
       // printf("setting %s\n", name.c_str());
        glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }


    inline GLuint getID(){ return m_shaderProgram; }

    inline GLuint getShaderAttrLocation(const char *name)
    {
        return glGetUniformLocation(m_shaderProgram, name );
    }


private:

    GLuint m_shaderProgram = 0;

    bool freeShader = true;

    const GLsizei maxNameLength = 28;
    std::unordered_map<std::string, GLuint> uniforms;



    std::string slurp(std::ifstream& in) {
        std::stringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
    }

    std::string getFileString(const char * file_path)
    {
        std::ifstream in(file_path);
        std::stringstream sstr;
        sstr << in.rdbuf();
        return sstr.str();
    }

};
