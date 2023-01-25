TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -static-libstdc++ \
    -static-libgcc

SOURCES += \
        Action_Processor.cpp \
        Archive.cpp \
        Archive_Manager.cpp \
        Args_Parser.cpp \
        File.cpp \
        HCoder.cpp \
        main.cpp

HEADERS += \
	Action_Processor.h \
	Archive.h \
	Archive_Manager.h \
	Args_Parser.h \
	File.h \
	HCoder.h
