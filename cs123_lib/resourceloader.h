#ifndef SHADER_H
#define SHADER_H
#include "CS123Common.h"

class ResourceLoader {
public:
    ResourceLoader();
    static GLuint loadShaders(const char *vertFile, const char *fragFile);

private:
    static GLuint loadShader(const char *file, int type);
};

#endif // SHADER_H
