#QMAKE_CXX	= "ccache $$QMAKE_CXX"
QMAKE_CXXFLAGS	+= -std=c++11

QT		+= core gui widgets printsupport network sql help

INCLUDEPATH     += ext/qcp ext/hid

SOURCES		+= mainwindow.cpp usb.cpp about.cpp donation.cpp update.cpp setup.cpp help.cpp ext/qcp/qcustomplot.cpp ext/qcp/qcpdocumentobject.cpp
HEADERS		+= mainwindow.h   usb.h   about.h   donation.h   update.h   setup.h   help.h   ext/qcp/qcustomplot.h   ext/qcp/qcpdocumentobject.h   ext/hid/hidapi.h

FORMS		+= res/ui/mainwindow.ui res/ui/usb.ui res/ui/about.ui res/ui/donation.ui res/ui/update.ui res/ui/setup.ui res/ui/help.ui
RESOURCES	+= res/obpm.qrc

TRANSLATIONS	+= lng/obpm_de.ts

unix:!macx {
SOURCES		+= ext/hid/hid-lin.c
LIBS		+= -ludev
QMAKE_LFLAGS	+= -Wl,-rpath=.
}

win32 {
SOURCES		+= ext/hid/hid-win.c
LIBS		+= -lsetupapi
RC_ICONS	+= res/ico/app.ico
}

macx {
SOURCES		+= ext/hid/hid-mac.c
LIBS		+= -framework IOKit -framework CoreFoundation
QMAKE_CXXFLAGS	+= -stdlib=libc++
ICON		+= res/ico/app.icns
}
