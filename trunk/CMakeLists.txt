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
FIND_PACKAGE(OpenSSL REQUIRED)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(includes)

#SET(DATADIR)

MESSAGE(" * Install in ${CMAKE_INSTALL_PREFIX} (${DATA_INSTALL_DIR}).")
SET(DATADIR "${CMAKE_INSTALL_PREFIX}/share/kapture")
ADD_DEFINITIONS(-DDATADIR=\\"${DATADIR}\\")

SET(Kapture_UIS
	src/gui/kapturewin.ui
	src/gui/mainframewin.ui
	src/gui/xmppwin.ui
	src/gui/chatwin.ui
	src/gui/xmppconfigdialog.ui
	src/gui/incomingfiledialog.ui
)
QT4_WRAP_UI(Kapture_UIS_H ${Kapture_UIS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(Kapture_OBJECTS
	includes/client.h
	includes/xmppwin.h
	includes/kapturewin.h
	includes/webcam.h
	includes/mainframewin.h
	includes/xmpp.h
	includes/tlshandler.h
	includes/stanza.h
	includes/rostermodel.h
	includes/chatwin.h
	includes/mouserostertableview.h
	includes/contact.h
	includes/xmppconfigdialog.h
	includes/profilemodel.h
	includes/mouseprofilestableview.h
	includes/task.h
	includes/tasks.h
	includes/socks5.h
	includes/incomingfiledialog.h
	includes/joystick.h
	includes/xmppreg.h
	includes/rosterdelegate.h
)
QT_WRAP_CPP(Kapture MOC_FILES ${Kapture_OBJECTS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${UVC_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
#Search also in ${CMAKE_INSTALL_PREFIX}
FIND_LIBRARY(SSL_LIBRARY NAME ssl PATHS /usr/lib /usr/local/lib)

LINK_LIBRARIES(${SSL_LIBRARY} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
	       ${QT_QTXML_LIBRARIES})

SET(Kapture_SRCS
	src/main.cpp
	src/imageConvert.cpp
	src/merror.cpp
	src/webcam.cpp
	src/emoticons.cpp
	src/config.cpp
	src/profile.cpp
	src/profilemodel.cpp
	src/joystick.cpp
	src/client/client.cpp
	src/client/xmpp.cpp
	src/client/xmlhandler.cpp
	src/client/event.cpp
	src/client/stanza.cpp
	src/client/contact.cpp
	src/client/jid.cpp
	src/client/vcard.cpp
	src/client/task.cpp
	src/client/tasks.cpp
	src/client/roster.cpp
	src/client/presence.cpp
	src/client/message.cpp
	src/client/xmppreg.cpp
	src/gui/xmppwin.cpp
	src/gui/kapturewin.cpp
	src/gui/mainframewin.cpp
	src/gui/mouselabel.cpp
	src/gui/rostermodel.cpp
	src/gui/chatwin.cpp
	src/gui/mouserostertableview.cpp
	src/gui/xmppconfigdialog.cpp
	src/gui/mouseprofilestableview.cpp
	src/gui/filetransferwidget.cpp
	src/gui/incomingfiledialog.cpp
	src/gui/rosterdelegate.cpp
	src/net/tlshandler.cpp
	src/net/socks5.cpp
)

ADD_EXECUTABLE(Kapture ${Kapture_SRCS} ${Kapture_UIS_H} ${MOC_FILES})
target_link_libraries(Kapture ${QT_LIBRARIES})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/Kapture DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES pics/smile.png DESTINATION ${DATADIR}/emoticons)
install(FILES 
  pics/decrypted.png
  pics/encrypted.png
  pics/offline.png
  pics/online.png
  pics/online-none.png
  pics/online-to.png
  pics/online-from.png
  pics/chat.png
  pics/xa.png
  pics/dnd.png
  pics/away.png
  pics/16x16.png
  pics/32x32.png
  pics/64x64.png
  pics/128x128.png
  DESTINATION ${DATADIR}/icons)
