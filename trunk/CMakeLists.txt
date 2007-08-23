PROJECT(Kapture)

if(UVCVIDEO)
	MESSAGE(" * Using UVCVIDEO driver.")
	SET(UVC_INCLUDE ${UVCVIDEO})
	ADD_DEFINITIONS(-DUSE_UVCVIDEO)
else (UVCVIDEO)
	MESSAGE(" * Not Using UVCVIDEO driver.")
	MESSAGE(" * If you want to, re-run cmake -D UVCVIDEO=/path/to/uvcvideo /path/to/Kapture/source")
ENDIF(UVCVIDEO)

ADD_DEFINITIONS("-g -O2 -fno-inline -W -Wall -Wextra")

FIND_PACKAGE(Qt4 4.3 REQUIRED)

INCLUDE(${QT_USE_FILE})

SET(Kapture_UIS kapturewin.ui mainframewin.ui xmppwin.ui chatwin.ui
xmppconfigdialog.ui)
QT4_WRAP_UI(Kapture_UIS_H ${Kapture_UIS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(Kapture_OBJECTS client.h xmppwin.h kapturewin.h webcam.h mainframewin.h xmpp.h
tlshandler.h stanza.h rosterModel.h chatwin.h mouserostertableview.h contact.h
xmppconfigdialog.h profilemodel.h mouseprofilestableview.h task.h tasks.h)
QT_WRAP_CPP(Kapture MOC_FILES ${Kapture_OBJECTS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${UVC_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
FIND_LIBRARY(SSL_LIBRARY NAME ssl PATHS /usr/lib /usr/local/lib)
LINK_LIBRARIES(${SSL_LIBRARY} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
	       ${QT_QTXML_LIBRARIES})

SET(Kapture_SRCS main.cpp imageConvert.cpp client.cpp kapturewin.cpp
mainframewin.cpp merror.cpp mouselabel.cpp webcam.cpp xmppwin.cpp xmpp.cpp
tlshandler.cpp xmlhandler.cpp event.cpp stanza.cpp rosterModel.cpp
chatwin.cpp mouserostertableview.cpp contact.cpp utils.cpp jid.cpp
xmppconfigdialog.cpp config.cpp profile.cpp profilemodel.cpp
mouseprofilestableview.cpp vcard.cpp task.cpp tasks.cpp roster.cpp
presence.cpp message.cpp)

ADD_EXECUTABLE(Kapture ${Kapture_SRCS} ${Kapture_UIS_H} ${MOC_FILES})
target_link_libraries(Kapture ${QT_LIBRARIES})
