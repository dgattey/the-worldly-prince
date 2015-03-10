#ifndef SHADER_H
#define SHADER_H
#include "GLCommon.h"
#include <QXmlStreamReader>

class ResourceLoader {
public:
    ResourceLoader();
    static GLuint loadShaders(const char *vertFile, const char *fragFile);
    static QString fileToString(const char *file);
    static QString copyFileToLocalData(const char *filePath);

private:
    static GLuint loadShader(const char *file, int type);
};

#endif // SHADER_H
