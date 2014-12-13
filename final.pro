QT += core gui opengl

TARGET = final
TEMPLATE = app

unix:!macx: LIBS += -lGLU

INCLUDEPATH += src glm cs123_lib shapes
DEPENDPATH += src glm cs123_lib shapes

SOURCES += src/settings.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/glwidget.cpp \
    cs123_lib/transforms.cpp \
    cs123_lib/resourceloader.cpp \
    src/camera.cpp \
    texquad.cpp \
    particleemitter.cpp \
    particle.cpp \
    shapes/shape.cpp \
    shapes/sphere.cpp \
    shapes/cylinder.cpp \
    shapes/flower.cpp

HEADERS += \
    src/settings.h \
    src/mainwindow.h \
    src/glwidget.h \
    src/camera.h \
    cs123_lib/transforms.h \
    cs123_lib/resourceloader.h \
    src/newmath.h \
    texquad.h \
    particleemitter.h \
    particle.h \
    shapes/shape.h \
    shapes/sphere.h \
    shapes/cylinder.h \
    shapes/flower.h

FORMS += src/mainwindow.ui

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

RESOURCES += \
    shaders/shaders.qrc


win32:CONFIG(release, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/release/ -lGLEW
else:win32:CONFIG(debug, debug|release): LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/debug/ -lGLEW
else:unix: LIBS += -L/course/cs123/lib/glew/glew-1.10.0/lib/ -lGLEW

INCLUDEPATH += /course/cs123/lib/glew/glew-1.10.0/include
DEPENDPATH += /course/cs123/lib/glew/glew-1.10.0/include

#DEFINES += GLEW_STATIC
#LIBS += -lglew -lopengl32

##includes are working correctly
#INCLUDEPATH+=C:\Users\Aisha\Documents\cs123\glew-1.11.0\include
#DEPENDPATH+=C:\Users\Aisha\Documents\cs123\glew-1.11.0\include
