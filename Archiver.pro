TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Archive.cpp \
        Archive_Manager.cpp \
        Args_Parser.cpp \
        HCoder.cpp \
        main.cpp

HEADERS += \
	Action_Processor.h \
	Archive.h \
	Archive_Manager.h \
	Args_Parser.h \
	HCoder.h
