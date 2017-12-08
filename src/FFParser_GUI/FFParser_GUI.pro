#-------------------------------------------------
#
# Project created by QtCreator 2017-10-12T15:44:25
#
#-------------------------------------------------


CONFIG += c++11

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFParser_GUI
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


INCLUDEPATH += "source/"

SOURCES += \
    source/main.cpp \
    source/export.cpp \
    source/mainwindow.cpp \
    source/contextmenu.cpp \
    source/changelanguage.cpp \
    source/exportcachefiledialog.cpp

TRANSLATIONS = languages/TranslationExample_en.ts  languages/TranslationExample_ru.ts

HEADERS += \
    source/mainwindow.h \
    source/export.h \
    source/contextmenu.h \
    source/changelanguage.h \
    source/exportcachefiledialog.h

FORMS += \
    source/mainwindow.ui \
    source/export.ui \
    source/exportcachefiledialog.ui

# Configuration settings
win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
	CONFIG(debug, debug|release) {
	    debug:DESTDIR = ../../bin/Debug/Win32
	    debug:OBJECTS_DIR = Debug/Win32/.obj
	    debug:MOC_DIR = Debug/Win32/.moc
	    debug:RCC_DIR = Debug/Win32/.rcc
	    debug:UI_DIR = Debug/Win32/.ui
	} else {
	    release:DESTDIR = ../../bin/Release/Win32
	    release:OBJECTS_DIR = Release/Win32/.obj
	    release:MOC_DIR = Release/Win32/.moc
	    release:RCC_DIR = Release/Win32/.rcc
	    release:UI_DIR = Release/Win32/.ui
	}
    } else {
	CONFIG(debug, debug|release) {
	    debug:DESTDIR = ../../bin/Debug/x64
	    debug:OBJECTS_DIR = Debug/x64/.obj
	    debug:MOC_DIR = Debug/x64/.moc
	    debug:RCC_DIR = Debug/x64/.rcc
	    debug:UI_DIR = Debug/x64/.ui
	} else {
	    release:DESTDIR = ../../bin/Release/x64
	    release:OBJECTS_DIR = Release/x64/.obj
	    release:MOC_DIR = Release/x64/.moc
	    release:RCC_DIR = Release/x64/.rcc
	    release:UI_DIR = Release/x64/.ui
	}
    }
}

