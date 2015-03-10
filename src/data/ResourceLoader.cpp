#include "ResourceLoader.h"
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>

ResourceLoader::ResourceLoader() {}

/**
 * @brief Given two file paths for a vert and frag shader, loads them in
 * Will load each shader separately, then create a program for both, attach
 * them, check them, and link them together. If all works well, will return
 * a GLuint for the program. If not, will print an error to stderr with an
 * appropriate message.
 * @param vertFile The vertex shader file
 * @param fragFile The fragment shader file
 * @return A GLuint representing the program
 */
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

/**
 * @brief Loads a shader in and returns a GLuint for it
 * Based on a filepath and a string representing type of shader,
 * loads the shader and returns a GLuint for it if it worked.
 * If not, outputs an error to stderr and returns a meaningless
 * GLuint.
 * @param path the file path
 * @param shaderType one of "vertex", "fragment", or "geometry"
 * @return a GLuint for the shader itself
 */
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
    std::string code = fileToString(path).toStdString();

    // Compile shader
    char const * ptr = code.c_str();
    glShaderSource(shaderID, 1, &ptr , NULL);
    glCompileShader(shaderID);

    // Check shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoSize);
    std::vector<char> error(infoSize);
    glGetShaderInfoLog(shaderID, infoSize, NULL, &error[0]);
    std::string perr(error.begin(),error.end());
    if (strlen(perr.c_str()) > 0) {
        fprintf(stderr, "Problem compiling %s shader: %s\n", type.c_str(), path);
        fprintf(stderr, "%s\n", &error[0]);
    }

    return shaderID;
}

/**
 * @brief Reads the file at the filepath into a string
 * @param path The filepath on the system
 * @return The contents of path if successful, or an error printed and "" if not
 */
QString ResourceLoader::fileToString(const char *path) {
    QString filePath = QString(path);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        fprintf(stderr, "Couldn't open file for reading: %s", path);
        return "";
    }
    QTextStream stream(&file);
    return stream.readAll();
}

/**
 * @brief Copies file at inPath to local data storage
 * @param inPath A path on the filesystem to a file to copy
 * @return The new path in the local app data folder
 */
QString ResourceLoader::copyFileToLocalData(const char *inPath) {
    QFile inFile(inPath);
    QFileInfo inInfo(inFile.fileName());
    QString filename = inInfo.fileName(); // Just the filename

    // Get local data path and create the folder if it doesn't exist already
    QString folder = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation).at(0);
    QDir::root().mkdir(folder);

    // Get the new location and the file corresponding to it
    QString outPath = folder + "/" + filename;
    QFile outFile(outPath);

    // Delete whatever file was there, copy new over, and set correct permissions
    if (outFile.exists()) outFile.remove();
    inFile.copy(outPath);
    outFile.setPermissions(QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther);

    // Return the new filepath as std::string
    return outPath;
}

