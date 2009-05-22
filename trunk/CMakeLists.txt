PROJECT(Kapture)

ADD_DEFINITIONS("-g -O0 -fno-inline -W -Wall -Wextra")

cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(Qt4 4.4 REQUIRED)
FIND_PACKAGE(OpenSSL REQUIRED)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(include)

MESSAGE(" * Install in ${CMAKE_INSTALL_PREFIX} (${DATA_INSTALL_DIR}).")
SET(DATADIR "${CMAKE_INSTALL_PREFIX}/share/kapture")
ADD_DEFINITIONS(-DDATADIR=\"${DATADIR}\")

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
	include/client.h
	include/xmppwin.h
	include/kapturewin.h
	include/webcam.h
	include/mainframewin.h
	include/xmpp.h
	include/tlshandler.h
	include/stanza.h
	include/rostermodel.h
	include/chatwin.h
	include/mouserostertableview.h
	include/contact.h
	include/xmppconfigdialog.h
	include/profilemodel.h
	include/mouseprofilestableview.h
	include/task.h
	include/tasks.h
	include/socks5.h
	include/incomingfiledialog.h
	include/joystick.h
	include/xmppreg.h
	include/rosterdelegate.h
	include/jinglestanza.h
	include/videowidget.h
)
QT_WRAP_CPP(Kapture MOC_FILES ${Kapture_OBJECTS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
#Search also in ${CMAKE_INSTALL_PREFIX}
FIND_LIBRARY(SSL_LIBRARY NAME ssl PATHS /usr/lib /usr/local/lib)

LINK_LIBRARIES(${SSL_LIBRARY} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
	       ${QT_QTXML_LIBRARIES})

SET(Kapture_SRCS
	src/main.cpp
	src/imageconvert.cpp
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
	src/client/jinglestanza.cpp
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
	src/gui/videowidget.cpp
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
