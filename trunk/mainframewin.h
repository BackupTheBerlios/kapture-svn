#ifndef MAINFRAMEWIN_H
#define MAINFRAMEWIN_H

#include <QMainWindow>
#include <QTimer>
#include <unistd.h>

#include "ui_mainframewin.h"
#include "webcam.h"

class MainFrameWin : public QMainWindow
{
	Q_OBJECT
public:
	MainFrameWin();
	~MainFrameWin();
	int posCurX;
	int posCurY;
	Ui::mainFrameWin ui;
//	QPixmap zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn);

public slots:
//	void getImage();
//	void changeFormat(const QString & itemSelected);
//	void getDeviceCapabilities();
//	void startStopVideo();
//	void savePhoto();
//	void crStartStop();
//	void colorChanged();
//	void showColorReplaced();
//	void satChanged();
//	void freqChanged();
//	void brightChanged();
//	void contChanged();
//	void sharpChanged();
//	void treatFrame();
//	int showZoom();
//	void keepZoomerTimeOut();
	
private:
//	Webcam *camera;

/*	QTimer waitCamera;
	QTimer keepZoomer;
	bool isCapturing;
	bool crIsActivated;
	QImage mainImageSav;
	QImage crImage;
	bool imageSaved;
	QImage imageFromCamera;
*/
};

#endif // MAINFRAMEWIN_H

