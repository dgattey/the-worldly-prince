#ifndef SHAPEDATA
#define SHAPEDATA

#include "GLCommon.h"

// Used in Ray Tracing for knowing how to compute the norm based on where the T came from
enum ShapePart {
    CONE_CAP,
    CONE_SIDE,
    CYLINDER_TOP_CAP,
    CYLINDER_BOTTOM_CAP,
    CYLINDER_SIDE,
    CUBE_FRONT,
    CUBE_BACK,
    CUBE_LEFT,
    CUBE_RIGHT,
    CUBE_TOP,
    CUBE_BOTTOM,
    SPHERE_P,
    NA
};

// Enumeration for types of primitives that can be stored in a scene file.
enum PrimitiveType {
    PRIMITIVE_CUBE,
    PRIMITIVE_CONE,
    PRIMITIVE_CYLINDER,
    PRIMITIVE_TORUS,
    PRIMITIVE_SPHERE,
    PRIMITIVE_MESH
};

// Struct to store a RGBA color in floats [0,1]
struct SceneColor {
    SceneColor() {}
    SceneColor(float r, float g, float b, float a) :
        r(r), g(g), b(b), a(a) {}

    union {
        struct {
           float r;
           float g;
           float b;
           float a;
        };
        float channels[4]; // points to the same four floats above...
    };

    SceneColor operator * (float scalar) {
        float nr = min(1.0f, max(0.0f, r*scalar));
        float ng = min(1.0f, max(0.0f, g*scalar));
        float nb = min(1.0f, max(0.0f, b*scalar));
        float na = min(1.0f, max(0.0f, a*scalar));
        return SceneColor(nr, ng, nb, na);
    }

    glm::vec3 vec() {
        return glm::vec3(r,g,b);
    }

    const SceneColor copy() {
        return SceneColor(r,g,b,a);
    }
};

// Data for file maps (ie: texture maps)
struct SceneFileMap {
   SceneFileMap() : texid(0) {}
   bool isUsed;
   std::string filename;
   float repeatU;
   float repeatV;
   GLuint texid;
};

// Data for scene materials
struct SceneMaterial {
   SceneMaterial() {}
   SceneMaterial(SceneColor d, SceneColor a, SceneColor r, SceneColor s,
                      SceneColor t, SceneColor e, SceneFileMap *tex, float b, SceneFileMap *bump, float shine, float ior)
       : cDiffuse(d), cAmbient(a), cReflective(r), cSpecular(s), cTransparent(t), cEmissive(e),
         blend(b), shininess(shine), ior(ior) {
       textureMap = new SceneFileMap();
       textureMap->filename = std::string(tex->filename);
       textureMap->isUsed = tex->isUsed;
       textureMap->repeatU = tex->repeatU;
       textureMap->repeatV = tex->repeatV;
       textureMap->texid = tex->texid;
       bumpMap = new SceneFileMap();
       bumpMap->filename =  std::string(bump->filename);
       bumpMap->isUsed = bump->isUsed;
       bumpMap->repeatU = bump->repeatU;
       bumpMap->repeatV = bump->repeatV;
       bumpMap->texid = bump->texid;
   }


   // This field specifies the diffuse color of the object. This is the color you need to use for
   // the object in sceneview. You can get away with ignoring the other color values until
   // intersect and ray.
   SceneColor cDiffuse;

   SceneColor cAmbient;
   SceneColor cReflective;
   SceneColor cSpecular;
   SceneColor cTransparent;
   SceneColor cEmissive;

   SceneFileMap* textureMap;
   float blend;

   SceneFileMap* bumpMap;

   float shininess;

   float ior; // index of refraction

   // To multiply in the global values
   SceneMaterial globalize(float ka, float kd) {
       SceneMaterial mat = copy();
       mat.cDiffuse = mat.cDiffuse * kd;
       mat.cAmbient = mat.cAmbient * ka;
       return mat;
   }

   SceneMaterial copy() {
       return SceneMaterial(cDiffuse.copy(), cAmbient.copy(), cReflective.copy(), cSpecular.copy(),
                   cTransparent.copy(), cEmissive.copy(), textureMap, blend, bumpMap, shininess, ior);
   }
};

// Used for ray tracing
typedef struct RayData {
    RayData() {
        t = INT_MAX;
        type = PRIMITIVE_MESH; // Default
        part = NA;
    }

    SceneMaterial material;
    PrimitiveType type;
    float t;
    glm::vec4 intersect;
    glm::vec4 intersectTransform;

    // Normals
    glm::vec4 norm;
    glm::mat3x3 transposedTransform;
    glm::mat4x4 transform;
    ShapePart part;

    glm::vec4 color; // final color
} RayData;

typedef struct TexturePointData {
    int w;
    int h;
    float u;
    float v;
} TexturePointData;

#endif // SHAPEDATA

