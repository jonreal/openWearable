QT += widgets

HEADERS       = rotation_display.h
SOURCES       = rotation_display.cpp \
                main.cpp

QMAKE_PROJECT_NAME = widgets_rotation_display

# install
target.path = ./bin/
INSTALLS += target
