# QtKeyboard project
# Major Version: 1
# 02.04.2021

# At least Qt 5.9 is required
# Tested on Qt 5.14.1 and Qt 5.9.0

include($${PWD}/../../common/QtKeyboard.pri)

QT       += core gui widgets

linux {
QT += x11extras
}

TARGET = qtkeyboard.bin
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
VERSION = 1.0.1.0  # Major.Minor.Update.Test
                   # Release version must have 0 Test and
                   # a non-0 Update
                   # Real effecive version may include a build or commit number
DEFINES += APPLICATION_VERSION=\\\"$$VERSION\\\"
#DEFINES += QT590

SOURCES += \
        $${SRCPATH}/core/main.cpp \
        $${SRCPATH}/core/applicationcontrol/exceptionhandlingapplication.cpp \
        $${SRCPATH}/core/applicationcontrol/keyboardlauncher.cpp \
        $${SRCPATH}/core/backend/base/abstractkeyboardbackend.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/mapping/directsetkeycodetotextconvertmappingstrategy.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/mapping/directsinglelayoutkeycodeonlytotextconvertmappingstrategy.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/mapping/keysetupdatedkeycodetotextconvertmappingstrategy.cpp \
        $${SRCPATH}/core/backend/keycodetotextconverting/mapping/singlelayoutkeycodeonlytotextconvertmappingstrategy.cpp \
        $${SRCPATH}/core/backend/keysets/keyboardkeyset.cpp \
        $${SRCPATH}/core/backend/keysets/factories/xml/xmlkeyboardkeysetfactory.cpp \
        $${SRCPATH}/core/errorhandling/errorhandler.cpp \
        $${SRCPATH}/core/utilities/keyboardutility.cpp \
        $${SRCPATH}/core/utilities/eventprocessingsignalwaiter.cpp \
        $${SRCPATH}/gui/frontend/keyboard.cpp \
        $${SRCPATH}/gui/animations/widgetslideanimator.cpp \
        $${SRCPATH}/gui/widgeteventfilters/abstractwidgeteventfilter.cpp \
        $${SRCPATH}/system/linux/x11/backend/x11backends/x11keyboardbackend.cpp \
        $${SRCPATH}/system/linux/x11/backend/x11backends/x11customeventhandlerkeyboardbackend.cpp \
        $${SRCPATH}/system/linux/x11/backend/eventhandlers/x11xrecordxkbdeventhandler.cpp \
        $${SRCPATH}/system/linux/x11/backend/keycodetotextconverting/x11keysymskeycodetotextconvertstrategy.cpp \
        $${SRCPATH}/system/linux/x11/backend/keycodetotextconverting/utilities/keysym2ucs.c \
        $${SRCPATH}/core/backend/eventhandlers/keyboardeventhandlerthreadeddecorator.cpp \
        $${SRCPATH}/system/linux/x11/backend/eventhandlers/x11xkbdtimerqueryingeventhandler.cpp \
        $${SRCPATH}/system/linux/x11/backend/eventhandlers/utilities/x11eventhandlersupportutility.cpp \
        $${SRCPATH}/system/linux/x11/backend/eventhandlers/abstractx11xkbdqueryingeventhandler.cpp \
        $${SRCPATH}/system/linux/x11/backend/eventhandlers/x11xkbdloopqueryingeventhandler.cpp

HEADERS +=  \
        $${INCLPATH}/core/applicationcontrol/exceptionhandlingapplication.h \
        $${INCLPATH}/core/applicationcontrol/keyboardlauncher.h \
        $${INCLPATH}/core/backend/base/abstractkeyboardbackend.h \
        $${INCLPATH}/core/backend/eventhandlers/abstractkeyboardeventhandler.h \
        $${INCLPATH}/core/backend/nullkeyboardbackend/nullkeyboardbackend.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/keycodetotextconvertstrategyinterface.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategytemplate.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/directsetkeycodetotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/directsinglelayoutkeycodeonlytotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/keysetupdatedkeycodetotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/singlelayoutkeycodeonlytotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keycodetotextconverting/mapping/modifiablekeycodetotextconvertmappingstrategy.h \
        $${INCLPATH}/core/backend/keysets/keyboardkeyset.h \
        $${INCLPATH}/core/backend/keysets/factories/keyboardkeysetfactoryinterface.h \
        $${INCLPATH}/core/backend/keysets/factories/xml/xmlkeyboardkeysetfactory.h \
        $${INCLPATH}/core/errorhandling/errorhandler.h \
        $${INCLPATH}/core/utilities/keyboardutility.h \
        $${INCLPATH}/core/utilities/eventprocessingsignalwaiter.h \
        $${INCLPATH}/core/utilities/customexitcodequiteventloop.h \
        $${INCLPATH}/gui/frontend/keyboard.h \
        $${INCLPATH}/gui/animations/widgetslideanimator.h \
        $${INCLPATH}/gui/widgeteventfilters/abstractwidgeteventfilter.h \
        $${INCLPATH}/system/linux/x11/backend/x11backends/x11keyboardbackend.h \
        $${INCLPATH}/system/linux/x11/backend/x11backends/x11customeventhandlerkeyboardbackend.h \
        $${INCLPATH}/system/linux/x11/backend/eventhandlers/x11xrecordxkbdeventhandler.h \
        $${INCLPATH}/system/linux/x11/backend/keycodetotextconverting/x11keysymskeycodetotextconvertstrategy.h \
        $${INCLPATH}/system/linux/x11/backend/keycodetotextconverting/utilities/keysym2ucs.h \
        $${INCLPATH}/core/backend/eventhandlers/keyboardeventhandlerthreadeddecorator.h \
        $${INCLPATH}/system/linux/x11/backend/eventhandlers/x11xkbdtimerqueryingeventhandler.h \
        $${INCLPATH}/system/linux/x11/backend/eventhandlers/utilities/x11eventhandlersupportutility.h \
        $${INCLPATH}/system/linux/x11/backend/eventhandlers/abstractx11xkbdqueryingeventhandler.h \
        $${INCLPATH}/system/linux/x11/backend/eventhandlers/x11xkbdloopqueryingeventhandler.h

linux {
LIBS += -lXtst -lX11
}

RESOURCES += \
    resources.qrc
