######################################################################
# Automatically generated by qmake (3.0) ?? ?? 15 17:37:40 2017
######################################################################
TEMPLATE = app
TARGET = uiviewer
CONFIG(x64){
TARGET = $$TARGET"64"
}

include($$(SOUI3PATH)/demo_com.pri)

dir = ..
CONFIG(debug, debug|release) {
	OBJECTS_DIR =   $$dir/obj/debug/$$TARGET
	DESTDIR = $$dir/bin
	CONFIG(x64){
		DESTDIR = $$DESTDIR"64"
	}	
	QMAKE_LIBDIR += $$DESTDIR
	TARGET = $$TARGET"d"
}
else {
	OBJECTS_DIR =   $$dir/obj/release/$$TARGET
	DESTDIR = $$dir/bin
	CONFIG(x64){
		DESTDIR = $$DESTDIR"64"
	}
	QMAKE_LIBDIR += $$DESTDIR
}


INCLUDEPATH += . \
		$(SOUI3PATH)/controls.extend \
		$(SOUI3PATH)/third-part/Scintilla/include \

CONFIG(debug,debug|release){
	LIBS += utilitiesd.lib soui3d.lib  ExtendCtrlsd.lib
}
else{
	LIBS += utilities.lib soui3.lib  ExtendCtrls.lib
}
LIBPATH += $$DESTDIR

PRECOMPILED_HEADER = stdafx.h
RC_FILE += uiviewer.rc


# Input
HEADERS += resource.h maindlg.h

SOURCES += uiviewer.cpp maindlg.cpp
