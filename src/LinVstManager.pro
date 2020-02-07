#-------------------------------------------------
#
# Project created by QtCreator 2019-08-10T12:24:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LinVstManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        confighandler.cpp \
        customprogressdialog.cpp \
        customsortfilterproxymodel.cpp \
        datahasher.cpp \
        dialogpreferences.cpp \
        dialogscan.cpp \
        horizontalline.cpp \
        legacyconfigparser.cpp \
        lineeditbridge.cpp \
        linkhandler.cpp \
        logoutput.cpp \
        main.cpp \
        mainwindow.cpp \
        modelscan.cpp \
        modelvstbuckets.cpp \
        preferences.cpp \
        runguard.cpp \
        scanhandler.cpp \
        scanresult.cpp \
        verticalline.cpp \
        vstbucket.cpp

HEADERS += \
        confighandler.h \
        customprogressdialog.h \
        customsortfilterproxymodel.h \
        datahasher.h \
        defines.h \
        dialogpreferences.h \
        dialogscan.h \
        enums.h \
        horizontalline.h \
        legacyconfigparser.h \
        lineeditbridge.h \
        linkhandler.h \
        logoutput.h \
        mainwindow.h \
        modelscan.h \
        modelvstbuckets.h \
        preferences.h \
        runguard.h \
        scanhandler.h \
        scanresult.h \
        verticalline.h \
        vstbucket.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \

RESOURCES += \
    resources/resources.qrc
