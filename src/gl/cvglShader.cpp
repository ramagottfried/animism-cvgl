
#include "cvglShader.hpp"

using namespace std;


int cvglShader::loadShaderFiles(const char * vertex_file_path, const char * fragment_file_path)
{
    string vertex_src = getFileString(vertex_file_path);
    string fragment_src = getFileString(fragment_file_path);
    
    if( !vertex_src.size() || !fragment_src.size() )
        return 0;
    
    if( loadShaders( vertex_src.c_str(), fragment_src.c_str() ) )
    {
        use();
        return 1;
    }
    
    return 0;
    
}

int cvglShader::loadShaderFiles(const string& vertex_file_path, const string& fragment_file_path)
{
    return loadShaderFiles(vertex_file_path.c_str(), fragment_file_path.c_str());
}


string getGLUniformTypeName(GLenum type)
{
    switch (type) {
        case GL_FLOAT:
            return "GL_FLOAT";
        case GL_FLOAT_VEC2:
            return "GL_FLOAT_VEC2";
        case GL_FLOAT_VEC3:
            return "GL_FLOAT_VEC3";
        case GL_FLOAT_VEC4:
            return "GL_FLOAT_VEC4";
        case GL_INT:
            return "GL_INT";
        case GL_INT_VEC2:
            return "GL_INT_VEC2";
        case GL_INT_VEC3:
            return "GL_INT_VEC3";
        case GL_INT_VEC4:
            return "GL_INT_VEC4";
        case GL_BOOL:
            return "GL_BOOL";
        case GL_BOOL_VEC2:
            return "GL_BOOL_VEC2";
        case GL_BOOL_VEC3:
            return "GL_BOOL_VEC3";
        case GL_BOOL_VEC4:
            return "GL_BOOL_VEC4";
        case GL_FLOAT_MAT2:
            return "GL_FLOAT_MAT2";
        case GL_FLOAT_MAT3:
            return "GL_FLOAT_MAT3";
        case GL_FLOAT_MAT4:
            return "GL_FLOAT_MAT4";
        case GL_SAMPLER_2D:
            return "GL_SAMPLER_2D";
        case GL_SAMPLER_CUBE:
            return "GL_SAMPLER_CUBE";
        default:
            return std::to_string(type);
    }
}

int cvglShader::loadShaders(const char * vertex_src, const char * fragment_src)
{
    
    GLuint m_vertexShader = 0;
    GLuint m_fragmentShader = 0;

    // Create and compile the vertex shader
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertexShader, 1, &vertex_src, NULL);
    glCompileShader(m_vertexShader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        std::cout << vertex_src << endl;
        return 0;
    }
    
    // Create and compile the fragment shader
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragmentShader, 1, &fragment_src, NULL);
    glCompileShader(m_fragmentShader);
    
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        std::cout << fragment_src << endl;
        return 0;
    }
    
    // Link the vertex and fragment shader into a shader program
    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);
    
    // check for linking errors
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    glDeleteShader(m_vertexShader);
    glDeleteShader(m_fragmentShader);


    glUseProgram(m_shaderProgram);


    GLint i;
    GLint count = 0;
    GLint size;
    GLenum type;
    const GLsizei bufSize = 28;
    GLchar name[bufSize];
    GLsizei length;

    glGetProgramiv(m_shaderProgram, GL_ACTIVE_UNIFORMS, &count);
    for( i = 0; i < count; i++ )
    {
        glGetActiveUniform(m_shaderProgram, (GLuint)i, bufSize, &size, &length, &type, name);

        std::cout << m_shaderProgram << " uniform " << i << " location " << glGetUniformLocation(m_shaderProgram, name) << " type " << getGLUniformTypeName(type) << " name " << name << std::endl;
        uniforms[name] = (GLuint)i;

        std::cout << "test uniform storage " << uniforms[name] << std::endl;
    }

    std::cout << "uniforms size " << uniforms.size() << endl;
    
    //glDetachShader(m_shaderProgram, m_vertexShader);
    //glDetachShader(m_shaderProgram, m_fragmentShader);

    
    return 1;
}
