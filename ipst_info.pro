TEMPLATE =	app

win32 :	DEFINES -= UNICODE
CONFIG += console boost164
CONFIG -= qt
CONFIG += \
          warn_on \ 
          thread 

unix {
    LIBS +=-ldl -lboost_filesystem -lboost_system -lboost_program_options -lreg_12
    TARGET=$${TARGET}.bin
}

win32 {
    LIBS += \
	-lws2_32 \
	-luser32
}

NITA_LIBS += \
#	-lBaseSet \
#	-lreg_12 \
#	-lOSIndependent

CONFIG(debug, debug|release) {
	OBJECTS_DIR = tmp/debug
	DEFINES += _DEBUG
	DEFINES -= NDEBUG


}
else {
	OBJECTS_DIR = tmp/release
	DEFINES -= _DEBUG
	DEFINES += NDEBUG
}

SOURCES += \
           main.cpp
