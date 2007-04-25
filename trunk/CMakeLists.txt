PROJECT(Kapture)

# Set it to your uvcvideo header files location
SET(UVC_INCLUDE /destination/of/uvcvideo )

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})

SET(Kapture_UIS kapturewin.ui mainframewin.ui xmppwin.ui)
QT4_WRAP_UI(Kapture_UIS_H ${Kapture_UIS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(Kapture_OBJECTS kapturewin.h webcam.h mainframewin.h xmpp.h tlsHandler.h)
QT_WRAP_CPP(Kapture MOC_FILES ${Kapture_OBJECTS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${UVC_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTNETWORK_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR})
FIND_LIBRARY(SSL_LIBRARY NAME ssl PATHS /usr/lib /usr/local/lib)
LINK_LIBRARIES(${SSL_LIBRARY} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARIES}
	       ${QT_QTXML_LIBRARIES})

SET(Kapture_SRCS main.cpp imageConvert.cpp kapturewin.cpp
mainframewin.cpp merror.cpp mouselabel.cpp webcam.cpp xmppwin.cpp xmpp.cpp
tlsHandler.cpp xmlHandler.cpp)

ADD_EXECUTABLE(Kapture ${Kapture_SRCS} ${Kapture_UIS_H} ${MOC_FILES})
target_link_libraries(Kapture ${QT_LIBRARIES})
