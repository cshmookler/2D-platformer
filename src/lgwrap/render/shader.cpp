#include "shader.h"

// shader error handler
int lgw::shaderHandleError(int error, char infoLog[lgwcon::SHADER_LOG_SIZE] = '\0')
{
    switch (error)
    {
    case lgwcon::SHADER_LOAD_FAILED: {
        std::cout << "gl: failed to open shader file" << std::endl;
        return error;
    }
    case lgwcon::SHADER_COMP_FAILED: {
        std::cout << "gl: failed to compile shader";
        break;
    }
    case lgwcon::SHADER_LINK_FAILED: {
        std::cout << "gl: failed to link shader";
        break;
    }
    default: {
        // do nothing
        return error;
    }
    }

    std::cout << std::endl << infoLog << std::endl;

    return error;
}

// vertex shader: constructor
lgw::VertexShader::VertexShader(GLenum type, const char* sourceDir) : dir(sourceDir), shaderType(type) {}
// vertex shader: destructor
lgw::VertexShader::~VertexShader(void) { glDeleteShader(id_); }
// vertex shader: load the contents of a file
std::string lgw::VertexShader::getFileContents(std::string dir)
{
    // open file
    std::ifstream file(dir);
    if (!file.is_open())
        return std::string(); // file not found
    // copy file contents to std::stringstream
    std::stringstream buffer;
    buffer << file.rdbuf();
    // close file
    file.close();
    return buffer.str(); // return file contents as a string
}
// vertex shader: compile from source
int lgw::VertexShader::compile(void)
{
    // get source from file directory
    std::string buffer = getFileContents(dir);
    const char* source = buffer.c_str();
    if (source == std::string())
        return shaderHandleError(lgwcon::SHADER_LOAD_FAILED);
    // compile shader
    id_ = glCreateShader(shaderType);
    glShaderSource(id_, 1, &source, NULL);
    glCompileShader(id_);
    int success;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[lgwcon::SHADER_LOG_SIZE];
        glGetShaderInfoLog(id_, lgwcon::SHADER_LOG_SIZE, NULL, infoLog);
        glDeleteShader(id_);
        id_ = 0;
        return shaderHandleError(lgwcon::SHADER_COMP_FAILED, infoLog);
    }
    return 0;
}

// shader program: constructor
lgw::Shader::Shader(VertexShader& vertexShader, FragmentShader& fragmentShader) : vertex(vertexShader.id), fragment(fragmentShader.id) {}
// shader program: destructor
lgw::Shader::~Shader(void) { glDeleteProgram(id_); }
// shader program: link vertex shader and fragment shader
int lgw::Shader::link(void)
{
    id_ = glCreateProgram();
    glAttachShader(id_, vertex);
    glAttachShader(id_, fragment);
    glLinkProgram(id_);
    int success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[lgwcon::SHADER_LOG_SIZE];
        glGetProgramInfoLog(id_, lgwcon::SHADER_LOG_SIZE, NULL, infoLog);
        id_ = 0;
        return shaderHandleError(lgwcon::SHADER_LINK_FAILED, infoLog);
    }
    return 0;
}
// shader program: use shader program
void lgw::Shader::use(void)
{
    glUseProgram(id_);
}
// shader program: get attribute location
GLint lgw::Shader::uniLoc(std::string attribute)
{
    return glGetUniformLocation(id_, attribute.c_str());
}