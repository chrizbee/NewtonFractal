QT += core gui widgets concurrent

TARGET = NewtonFractal
TEMPLATE = app
CONFIG += c++14
VERSION = 0.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG(release, debug|release) {
    OBJECTS_DIR = release/obj
    MOC_DIR = release/moc
    RCC_DIR = release/rcc
    UI_DIR = release/ui
}

CONFIG(debug, debug|release) {
    OBJECTS_DIR = debug/obj
    MOC_DIR = debug/moc
    RCC_DIR = debug/rcc
    UI_DIR = debug/ui
}

RC_ICONS = resources/icons/icon.ico
DESTDIR = build

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/fractalwidget.cpp \
    src/renderthread.cpp \
    src/parameters.cpp

HEADERS += \
    src/mainwindow.h \
    src/fractalwidget.h \
    src/renderthread.h \
    src/parameters.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
