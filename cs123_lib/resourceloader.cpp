#include "resourceloader.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>

ResourceLoader::ResourceLoader() {

}

GLuint ResourceLoader::loadShaders(const char *vertFile, const char * fragFile){
    GLint result = GL_FALSE;
    int infoSize;

    // Create the shaders
    GLuint vertShaderID = loadShader(vertFile, GL_VERTEX_SHADER);
    GLuint fragShaderID = loadShader(fragFile, GL_FRAGMENT_SHADER);

    // Link the program
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderID);
    glAttachShader(programId, fragShaderID);
    glLinkProgram(programId);

    // Check the program
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoSize);
    std::vector<char> programError(std::max(infoSize, int(1)));
    glGetProgramInfoLog(programId, infoSize, NULL, &programError[0]);

    // Error check print
    std::string perr(programError.begin(),programError.end());
    if (strlen(perr.c_str()) == 0) {
        fprintf(stdout, "Shaders %s and %s linked successfully!\n", vertFile, fragFile);
    } else {
        fprintf(stdout, "ERROR: %s and %s not linked\n", vertFile, fragFile);
    }

    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    return programId;
}

GLuint ResourceLoader::loadShader(const char *path, int shaderType) {
    GLint result = GL_FALSE;
    int infoSize;
    bool typeFine = true;
    std::string type = "";
    switch (shaderType) {
    case GL_VERTEX_SHADER:
        type = "vertex";
        break;
    case GL_FRAGMENT_SHADER:
        type = "fragment";
        break;
    case GL_GEOMETRY_SHADER:
        type = "geometry";
        break;
    default:
        fprintf(stderr, "Invalid shader type passed to loadShader");
        typeFine = false;
        break;
    }
    if (!typeFine) return -1;

    // Create the shader
    GLuint shaderID = glCreateShader(shaderType);

    // Read the shader code from the file
    std::string code;
    QString filePath = QString(path);
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        code = stream.readAll().toStdString();
    }

    // Compile shader
    char const * ptr = code.c_str();
    glShaderSource(shaderID, 1, &ptr , NULL);
    glCompileShader(shaderID);

    // Check shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoSize);
    std::vector<char> error(infoSize);
    glGetShaderInfoLog(shaderID, infoSize, NULL, &error[0]);
    if (&error[0]) {
        fprintf(stderr, "Problem compiling %s shader: %s\n", type.c_str(), path);
        fprintf(stderr, "%s\n", &error[0]);
    }

    return shaderID;
}

