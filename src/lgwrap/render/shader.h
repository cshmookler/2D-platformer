#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glad/glad.h>

// constants
namespace lgwcon {
    // general constants
    const int SHADER_LOG_SIZE = 1024;
    // error codes
    const int SHADER_LOAD_FAILED = 1;
    const int SHADER_COMP_FAILED = 2;
    const int SHADER_LINK_FAILED = 3;
}

namespace lgw {
    // error handler
    int shaderHandleError(int error, char infoLog[lgwcon::SHADER_LOG_SIZE]);

    // vertex shader
    class VertexShader {
    private:
        // source directory
        const char* dir;
        // shader ID
        GLuint id_ = 0;
        // type of shader
        GLenum shaderType;
    public:
        // return source from a file
        std::string getFileContents(std::string dir);
        // shader ID reference
        const GLuint& id = id_;
        // constructor
        VertexShader(GLenum type, const char* sourceDir);
        // destructor
        ~VertexShader(void);
        // compile from source
        int compile(void);
    };
    // fragment shader (identical to a vertex shader in all but name)
    typedef VertexShader FragmentShader;

    // shader program
    class Shader {
    private:
        // shader ID
        GLuint id_ = 0;
        // vertex shader ID
        GLuint vertex;
        // fragment shader ID
        GLuint fragment;
    public:
        // shader ID reference
        const GLuint& id = id_;
        // constructor
        Shader(VertexShader& vertexShader, FragmentShader& fragmentShader);
        // destructor
        ~Shader(void);
        // link vertex shader and fragment shader
        int link(void);
        // use shader program
        void use(void);
        // get attribute location (std::string)
        GLint uniLoc(std::string attribute);
    };
}