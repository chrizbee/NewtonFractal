QT += core gui widgets concurrent

TARGET = NewtonFractal
TEMPLATE = app
CONFIG += c++14 debug_and_release
VERSION = 1.3.3
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
    src/fractalwidget.cpp \
    src/renderthread.cpp \
    src/parameters.cpp \
    src/rootedit.cpp \
    src/limits.cpp \
    src/sizeedit.cpp \
    src/settingswidget.cpp \
    src/imageline.cpp

HEADERS += \
    src/fractalwidget.h \
    src/renderthread.h \
    src/parameters.h \
    src/rootedit.h \
    src/defaults.h \
    src/limits.h \
    src/sizeedit.h \
    src/settingswidget.h \
    src/imageline.h

FORMS += \
    src/settingswidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
