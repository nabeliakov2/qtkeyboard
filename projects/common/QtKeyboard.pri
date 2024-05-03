CONFIG += debug_and_release
CONFIG += c++14

PROJECTS_DIR = $${PWD}/..
ROOT_DIR = $${PWD}/../..
SRCPATH = $${ROOT_DIR}/src
INCLPATH = $${ROOT_DIR}/include
INCLUDEPATH += $${ROOT_DIR}/include
CURRENT_ROOT_DIR = $${_PRO_FILE_PWD_}/../../qmake
CURRENT_ROOT_DIR_ALL = $${_PRO_FILE_PWD_}/../..

MOC_DIR = $${CURRENT_ROOT_DIR}/build/moc
UI_DIR = $${CURRENT_ROOT_DIR}/build/ui
RCC_DIR = $${CURRENT_ROOT_DIR}/build/rcc
linux {
    message("Linux")
    build_pass:CONFIG(debug, debug|release) {
        DESTDIR = $${CURRENT_ROOT_DIR}/bin/linux/debug
        OBJECTS_DIR = $${CURRENT_ROOT_DIR}/build/linux/debug
    }
    build_pass:CONFIG(release, debug|release) {
        DESTDIR = $${CURRENT_ROOT_DIR}/bin/linux/release
        OBJECTS_DIR = $${CURRENT_ROOT_DIR}/build/linux/release
    }
}
windows {
    message("Windows")
}
