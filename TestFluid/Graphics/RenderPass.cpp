#include "RenderPass.h"

RenderPass::RenderPass()
{
}

void RenderPass::addVertexShader(const char* vertexShaderSource)
{
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    p_shaderStages.push_back(vertexShader);
}

void RenderPass::addFragmentShader(const char* fragmentShaderSource)
{
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    p_shaderStages.push_back(fragmentShader);
}

void RenderPass::addGeometryShader(const char* geometryShaderSource)
{
    // fragment shader
    unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    p_shaderStages.push_back(geometryShader);
}

void RenderPass::addComputeShader(const char* computeShaderSource)
{
    // fragment shader
    unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(computeShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    p_shaderStages.push_back(computeShader);
}

void RenderPass::linkShaders()
{
    // link shaders
    shaderProgram = glCreateProgram();
    for (int i = 0; i < p_shaderStages.size(); i++)
    {
        glAttachShader(shaderProgram, p_shaderStages.at(i));
    }
    glLinkProgram(shaderProgram);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete shader objects that arn't needed:
    for (int i = 0; i < p_shaderStages.size(); i++)
    {
        glDeleteShader(p_shaderStages.at(i));
    }
}

void RenderPass::use()
{
    glUseProgram(shaderProgram);
}

RenderPass::~RenderPass()
{
    glDeleteProgram(shaderProgram);
}