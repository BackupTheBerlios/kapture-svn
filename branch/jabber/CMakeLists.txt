
PROJECT(Kapture)
INCLUDE_DIRECTORIES(/usr/include /usr/share/qt4/mkspecs/linux-g++
/usr/include/qt4/QtCore /usr/include/qt4/QtGui /usr/include/qt4)

SET(QT_MOC_EXECUTABLE /usr/bin/moc)
#Must generate ui_kapturewin.h and ui_mainframewin.h files.
SET(Kapture_UIS kapturewin.ui mainframewin.ui)
QT_WRAP_UI(Kapture_UIS_H ${Kapture_UIS})
QT_WRAP_CPP(robotbuilder MOC_FILES kapturewin.h webcam.h mainframewin.h)

ADD_EXECUTABLE(Kapture main.cpp imageConvert.cpp kapturewin.cpp
mainframewin.cpp merror.cpp meteoplugin.cpp mouselabel.cpp webcam.cpp ${MOC_FILES} )

FIND_LIBRARY(LIBRARIES QtGui QtCore PATHS /usr/lib/qt4 /usr/lib)
MESSAGE(${LIBRARIES})
ADD_LIBRARY(Kapture STATIC ${MOC_FILES})
TARGET_LINK_LIBRARIES(Kapture ${LIBRARIES} )

