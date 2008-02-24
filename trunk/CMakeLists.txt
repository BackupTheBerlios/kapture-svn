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

#SET(DATADIR)

MESSAGE(" * Install in ${CMAKE_INSTALL_PREFIX} (${DATA_INSTALL_DIR}).")
SET(DATADIR "${CMAKE_INSTALL_PREFIX}/share/kapture")
ADD_DEFINITIONS(-DDATADIR=\\"${DATADIR}\\")

SET(Kapture_UIS kapturewin.ui mainframewin.ui xmppwin.ui chatwin.ui
xmppconfigdialog.ui incomingfiledialog.ui)
QT4_WRAP_UI(Kapture_UIS_H ${Kapture_UIS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(Kapture_OBJECTS client.h xmppwin.h kapturewin.h webcam.h mainframewin.h xmpp.h
tlshandler.h stanza.h rosterModel.h chatwin.h mouserostertableview.h contact.h
xmppconfigdialog.h profilemodel.h mouseprofilestableview.h task.h tasks.h
socks5.h incomingfiledialog.h joystick.h xmppreg.h rosterdelegate.h)
QT_WRAP_CPP(Kapture MOC_FILES ${Kapture_OBJECTS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${UVC_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
#Search also in ${CMAKE_INSTALL_PREFIX}
FIND_LIBRARY(SSL_LIBRARY NAME ssl PATHS /usr/lib /usr/local/lib)

LINK_LIBRARIES(${SSL_LIBRARY} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
	       ${QT_QTXML_LIBRARIES})

SET(Kapture_SRCS main.cpp imageConvert.cpp client.cpp kapturewin.cpp
mainframewin.cpp merror.cpp mouselabel.cpp webcam.cpp xmppwin.cpp xmpp.cpp
tlshandler.cpp xmlhandler.cpp event.cpp stanza.cpp rosterModel.cpp
chatwin.cpp mouserostertableview.cpp contact.cpp emoticons.cpp jid.cpp
xmppconfigdialog.cpp config.cpp profile.cpp profilemodel.cpp
mouseprofilestableview.cpp vcard.cpp task.cpp tasks.cpp roster.cpp
presence.cpp message.cpp socks5.cpp filetransferwidget.cpp
incomingfiledialog.cpp joystick.cpp xmppreg.cpp rosterdelegate.cpp)

ADD_EXECUTABLE(Kapture ${Kapture_SRCS} ${Kapture_UIS_H} ${MOC_FILES})
target_link_libraries(Kapture ${QT_LIBRARIES})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/Kapture DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES smile.png DESTINATION ${DATADIR}/emoticons)
install(FILES 
  decrypted.png
  encrypted.png
  offline.png
  online.png
  online-none.png
  online-to.png
  online-from.png
  chat.png
  xa.png
  dnd.png
  away.png
  16x16.png
  32x32.png
  64x64.png
  128x128.png
  DESTINATION ${DATADIR}/icons)
