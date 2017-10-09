#-------------------------------------------------
#
# Project created by QtCreator 2013-05-21T14:32:29
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = fouriergl
TEMPLATE = app

win32: LIBS += opengl32.lib
linux: LIBS += -L /usr/lib/ -lQtOpenGL

SOURCES += main.cpp\
        dialog.cpp \
    cglwidget.cpp \
    utils.cpp \
    ccircle.cpp

HEADERS  += dialog.h \
    cglwidget.h \
    utils.h \
    ccircle.h

FORMS    += dialog.ui

RESOURCES += \
    res.qrc

OTHER_FILES += \
    vshader.vert \
    fshader.frag

DISTFILES += \
    gshader.geom \
    outline.vert \
    outline.frag
