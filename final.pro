QT += core gui opengl
TARGET = final
TEMPLATE = app

unix:!macx {
    LIBS += -lGLU
    QMAKE_CXXFLAGS += -std=c++11
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}

SOURCES += src/settings.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    cs123_lib/transforms.cpp \
    cs123_lib/resourceloader.cpp \
    src/camera.cpp \
    shapes/shape.cpp \
    shapes/sphere.cpp \
    shapes/cylinder.cpp \
    shapes/flower.cpp \
    particles/particle.cpp \
    particles/texquad.cpp \
    src/databinding.cpp

HEADERS += \
    src/settings.h \
    src/mainwindow.h \
    src/glwidget.h \
    src/camera.h \
    cs123_lib/transforms.h \
    cs123_lib/resourceloader.h \
    src/newmath.h \
    shapes/shape.h \
    shapes/sphere.h \
    shapes/cylinder.h \
    shapes/flower.h \
    cs123_lib/CS123Common.h \
    particles/particle.h \
    particles/particleemitter.h \
    particles/texquad.h \
    src/databinding.h

FORMS += src/mainwindow.ui
INCLUDEPATH += particles src glm cs123_lib shapes
DEPENDPATH += particles src glm cs123_lib shapes
DEFINES += TIXML_USE_STL

OTHER_FILES += \
    shaders/flower.frag \
    shaders/flower.vert \
    shaders/brightpass.frag \
    shaders/lightblur.frag \
    shaders/tex.vert \
    shaders/tex.frag \
    shaders/lights.frag \
    shaders/lights.vert \
    shaders/star.frag \
    shaders/star.vert \
    shaders/noise.frag \
    shaders/noise.vert

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}


# These lines needed for Linux
#win32:CONFIG(release, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/release/ -lGLEW
#else:win32:CONFIG(debug, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/debug/ -lGLEW
#else:unix: LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/ -lGLEW
#INCLUDEPATH += /course/cs123/lib/glew/glew-1.10.0/include
#DEPENDPATH += /course/cs123/lib/glew/glew-1.10.0/include

# These lines needed for Mac
win32:CONFIG(release, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/release/ -lGLEW
else:win32:CONFIG(debug, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/debug/ -lGLEW
else:unix: LIBS += -L/usr/local/Cellar/glew/1.11.0/lib/ -lGLEW
INCLUDEPATH+=/usr/local/Cellar/glew/1.11.0/include
DEPENDPATH+=/usr/local/Cellar/glew/1.11.0/include

# These lines needed for Windows
#DEFINES += GLEW_STATIC
#LIBS += -lglew -lopengl32
##includes are working correctly
#INCLUDEPATH+=C:\Users\Aisha\Documents\cs123\glew-1.11.0\include
#DEPENDPATH+=C:\Users\Aisha\Documents\cs123\glew-1.11.0\include

RESOURCES += \
    shaders/shaders.qrc
