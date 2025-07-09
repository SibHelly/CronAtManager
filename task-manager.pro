QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

SOURCES += \
    atcreatediolog.cpp \
    atmanager.cpp \
    atparseexception.cpp \
    atparser.cpp \
    croncreatediolog.cpp \
    cronmanager.cpp \
    cronparseexception.cpp \
    cronparser.cpp \
    isystemexecutor.cpp \
    main.cpp \
    mainwindow.cpp \
    systemexecutionexception.cpp \
    systemexecutor.cpp \
    taskschedulerexception.cpp \
    taskschedulerservice.cpp \
    utils.cpp

HEADERS += \
    atcreatediolog.h \
    atmanager.h \
    atparseexception.h \
    atparser.h \
    basicStructs.h \
    croncreatediolog.h \
    cronmanager.h \
    cronparseexception.h \
    cronparser.h \
    globals.h \
    isystemexecutor.h \
    mainwindow.h \
    systemexecutionexception.h \
    systemexecutor.h \
    taskschedulerexception.h \
    taskschedulerservice.h \
    utils.h

FORMS += \
    atcreatediolog.ui \
    croncreatediolog.ui \
    mainwindow.ui

# Installation paths
isEmpty(PREFIX) {
    PREFIX = /usr/local
}

target.path = $$PREFIX/bin
INSTALLS += target
