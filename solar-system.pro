QT += core gui opengl
TARGET = "The Little Prince"
TEMPLATE = app

INCLUDEPATH += src src/data src/lib src/render src/scene src/shapes
DEPENDPATH += src src/data src/lib src/render src/scene src/shapes

SOURCES += \
    src/data/Bindings.cpp \
    src/data/ResourceLoader.cpp \
    src/data/Settings.cpp \
    src/data/Window.cpp \
    src/render/FlowersRenderer.cpp \
    src/render/GLRenderWidget.cpp \
    src/render/PlanetsRenderer.cpp \
    src/render/StarsRenderer.cpp \
    src/scene/Camera.cpp \
    src/scene/Particle.cpp \
    src/scene/TexturedQuad.cpp \
    src/scene/Transforms.cpp \
    src/shapes/Cone.cpp \
    src/shapes/Cube.cpp \
    src/shapes/Cylinder.cpp \
    src/shapes/Flower.cpp \
    src/shapes/Shape.cpp \
    src/shapes/Sphere.cpp \
    src/main.cpp \
    src/data/PlanetDataParser.cpp

HEADERS += \
    src/data/Bindings.h \
    src/data/ResourceLoader.h \
    src/data/Settings.h \
    src/data/ShapeData.h \
    src/data/Window.h \
    src/lib/GLCommon.h \
    src/lib/GLMath.h \
    src/render/FlowersRenderer.h \
    src/render/GLRenderWidget.h \
    src/render/PlanetsRenderer.h \
    src/render/Renderer.h \
    src/render/StarsRenderer.h \
    src/scene/Camera.h \
    src/scene/Particle.h \
    src/scene/TexturedQuad.h \
    src/scene/Transforms.h \
    src/shapes/Cone.h \
    src/shapes/Cube.h \
    src/shapes/Cylinder.h \
    src/shapes/Flower.h \
    src/shapes/Shape.h \
    src/shapes/Sphere.h \
    src/data/PlanetDataParser.h

FORMS += \
    src/data/Window.ui
RESOURCES += \
    resources/shaders.qrc \
    resources/images.qrc \
    resources/xml.qrc
DISTFILES +=

# Flags and compile options
DEFINES += TIXML_USE_STL
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

# Nice feature of copying up the target to the main folder
INSTALLS += target
target.path = $$PWD

# Change path to local installation as needed
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
    INCLUDEPATH += /usr/local/Cellar/glew/1.11.0/include
    DEPENDPATH += /usr/local/Cellar/glew/1.11.0/include
    LIBS += -L/usr/local/Cellar/glew/1.11.0/lib/ -lGLEW
    ICON = $${PWD}/resources/icons/mac.icns
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lglew -lopengl32
    INCLUDEPATH += C:\Users\Aisha\Documents\cs123\glew-1.11.0\include
    DEPENDPATH += C:\Users\Aisha\Documents\cs123\glew-1.11.0\include
}
unix:!macx {
    LIBS += -lGLU
    LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/release/ -lGLEW
}
