######################################################################
# Automatically generated by qmake (3.0) ?? 2? 5 22:13:23 2020
######################################################################
TEMPLATE = app
TARGET = uiviewer

include($$(SOUI4PATH)/demo_com.pri)

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
		$(SOUI4PATH)/controls.extend \
		$(SOUI4PATH)/third-part/Scintilla/include \

CONFIG(debug,debug|release){
	LIBS += utilities4d.lib soui4d.lib  ExtendCtrlsd.lib
}
else{
	LIBS += utilities4.lib soui4.lib  ExtendCtrls.lib
}
LIBPATH += $$DESTDIR

PRECOMPILED_HEADER = stdafx.h
RC_FILE += uiviewer.rc


# Input
HEADERS += Adapter.h \
           PreviewContainer.h \
           PreviewHost.h \
           resource.h 
           
SOURCES += Adapter.cpp \
           PreviewContainer.cpp \
           PreviewHost.cpp \
           uiviewer.cpp
