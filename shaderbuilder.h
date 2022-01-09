#pragma once

#include <GL/glew.h>

#include <stdio.h>
#include <vector>
#include <string>

class ShaderBuilder {
public:
    static GLuint           compileShader(const char* location, GLuint shaderType);

    static void             linkProgram(GLuint program);
    static GLuint           createComputeShaderProgram(const char* computeShaderLoc);
    static GLuint           createShaderProgram(const char* vertShaderLoc, const char* fragShaderLoc);
private:
    ShaderBuilder();
};
