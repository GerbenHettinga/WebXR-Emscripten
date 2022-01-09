#include "shaderbuilder.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#define DEBUG_SHADERS 1

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

std::string replaceFlags(std::string orig, int valency) {
    size_t index = orig.find("/* VLFLAG */");

    if (index != std::string::npos) {
        std::string replacement = "           " + std::to_string(valency);
        orig.replace(index, 12, replacement);
    }

    return orig;
}


ShaderBuilder::ShaderBuilder() {

}

GLuint ShaderBuilder::compileShader(const char* location, GLuint shaderType) {
    std::string text = readFile(location);
    const char *src = text.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled );
    if(DEBUG_SHADERS && !shaderCompiled)
        std::cout << shaderCompiled << " " << shaderType << std::endl;

    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength > 0) {
        std::vector<char> shaderError((logLength > 1) ? logLength : 1);
        glGetShaderInfoLog(shader, logLength, NULL, &shaderError[0]);
        if(DEBUG_SHADERS)
            std::cout << &shaderError[0] << std::endl;
    }

    return shader;
}

void ShaderBuilder::linkProgram(GLuint program) {
    glLinkProgram(program);

    GLint programLinked = GL_FALSE;
    glGetProgramiv( program, GL_LINK_STATUS, &programLinked );
    if(DEBUG_SHADERS && !programLinked)
        std::cout << programLinked << " program" << std::endl;

    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength > 0) {
        std::vector<char> programError((logLength > 1) ? logLength : 1);
        glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
        if(DEBUG_SHADERS)
            std::cout << &programError[0] << std::endl;
    }


    GLint error = glGetError();
    if(error) {
       std::cout << "render errors " << error << std::endl;
    }

}



GLuint ShaderBuilder::createShaderProgram(const char* vertShaderLoc, const char* fragShaderLoc) {
    GLuint vertShader = compileShader(vertShaderLoc, GL_VERTEX_SHADER);
    GLuint fragShader = compileShader(fragShaderLoc, GL_FRAGMENT_SHADER);


    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);

    linkProgram(program);

    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

