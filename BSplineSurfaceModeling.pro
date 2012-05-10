#-------------------------------------------------
#
# Project created by QtCreator 2012-01-13T14:24:01
#
#-------------------------------------------------

QT       += core gui\
            opengl

TARGET = 6870HW2
TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++0x

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    BSplineSurfaceView.cpp \
    BSplineSurface.cpp \
    ModelView.cpp \
    Vertex3d.cpp

HEADERS  += \
    mainwindow.h \
    BSplineSurfaceView.h \
    BSplineSurface.h \
    Vertex3d.h \
    ModelView.h



FORMS    += mainwindow.ui
