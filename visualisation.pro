#-------------------------------------------------
#
# Project created by QtCreator 2016-09-07T14:31:53
#
#-------------------------------------------------

QT       += core gui \
            opengl \
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visualisation
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/element.cpp \
    src/scene.cpp \
    src/model.cpp \
    src/manipulator.cpp \
    src/model_manager.cpp \
    src/control_panel.cpp \
    src/edit_window.cpp \
    src/element_type.cpp \
    src/octave_connection.cpp \
    src/settings_window.cpp \
    src/materials.cpp \
    src/dh_parameters.cpp \
    src/angle_spinbox.cpp \
    src/ik_dialog.cpp


HEADERS  += inc/mainwindow.h \
    inc/element.h \
    inc/scene.h \
    inc/constants.h \
    inc/model.h \
    inc/materials.h \
    inc/manipulator.h \
    inc/model_manager.h \
    inc/control_panel.h \
    inc/edit_window.h \
    inc/element_type.h \
    inc/octave_connection.h \
    inc/settings_window.h \
    inc/dh_parameters.h \
    inc/angle_spinbox.h \
    inc/ik_dialog.h

CONFIG += c++11

LIBS += -lglut

linux-g++ | linux-g++-64 | linux-g++-32 {
    LIBS += -lGLU
}

win32 {
    LIBS += -lglu32 -lopengl32
}

RESOURCES += \
    res.qrc

TRANSLATIONS = languages/visualisation_pl.ts

DISTFILES +=
