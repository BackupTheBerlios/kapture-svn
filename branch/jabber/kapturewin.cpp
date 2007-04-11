/*
 *      kapturewin.cpp -- Kapture
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include <unistd.h>
#include <QCheckBox>
#include <QAbstractButton>
#include <QImageReader>

#include "kapturewin.h"
#include "mainframewin.h"
#include "webcam.h"
#include "xmpp.h"

/*
 * This Software is at a developpement state.
 * This is under GNU Public License.
 * You can use, modify or redistribute it.
 *
 * FIXME:
 * 	- Add comments in the code.
 * 	- The timer can be replaced by Dbus (Qt >= 4.2)
 * TODO:
 * 	- Add more goodies (B&W, ...)
 * 	- Add support for more controls (Pan-tilts,...)
 */

int result = 0;
char formatString[128] = "00000x00000 (MINIMAL)"; // FIXME: use a QString()
QImage *imageZoomed = new QImage(60,60,QImage::Format_RGB32); 


KaptureWin::KaptureWin()
	: QMainWindow()
{
	Xmpp *jabberAccount = new Xmpp("test1@localhost");
	connect(jabberAccount,SIGNAL(needUserName()), SLOT(setXmppUserName()));
	if (jabberAccount->connected())
		jabberAccount->auth("123456789","Kapture");
/*
 * The above is commented while I'm testing xmpp.
 */

/*	
	int i = 0;
	otherVideoDevice = false;
	for( ;i < qApp->arguments().size(); i++)
	{
		printf("Element %i = %s\n", i, qApp->arguments().at(i).toLatin1().constData());
		if (qApp->arguments().at(i) == QString("-d"))
		{
			otherVideoDevice = true;
			videoDevice = qApp->arguments().at(i + 1);
		}
	}
	if (!otherVideoDevice)
		videoDevice = QString("/dev/video0");
	int defaultSat, defaultFreq, defaultBright, defaultCont, defaultSharp;	

	crIsActivated = false;
	ui.setupUi(this);
	ui.crBox->hide();
	fctExecuted = 0;

	mfw = new MainFrameWin();
	mfw->resize(320, 240);
	mfw->show();
	connect(ui.showMfwBtn,	SIGNAL(clicked()), mfw, SLOT(show()));
	
	ui.comboBoxSize->clear();
	
	camera = new Webcam();
	getDeviceCapabilities(); // Open device and fill the param's comboBoxes (doesn't close it)

	defaultSat = camera->defaultCtrlVal(0); // Saturation to default
	defaultFreq = camera->defaultCtrlVal(1);  // Frequency to default
	defaultBright =  camera->defaultCtrlVal(2);  // Brightness to default
	defaultCont = camera->defaultCtrlVal(3);  // Contrast to default
	defaultSharp = camera->defaultCtrlVal(4);  // Contrast to default
	camera->changeCtrl(0, defaultSat); // Saturation to default
	camera->changeCtrl(1, defaultFreq);  // Frequency to default
	camera->changeCtrl(2, defaultBright);  // Brightness to default
	camera->changeCtrl(3, defaultCont);  // Contrast to default
	camera->changeCtrl(4, defaultSharp);  // Contrast to default
	ui.satManualValueBox->setValue(defaultSat);
	ui.freqBox->setChecked((defaultFreq == 1) ? true : false);
	ui.brightManualValueBox->setValue(defaultBright);
	ui.contManualValueBox->setValue(defaultCont);
	ui.sharpManualValueBox->setValue(defaultSharp);

	crImage = QImage(320, 240, QImage::Format_RGB32);
	
	connect(ui.btnVideo,    SIGNAL(clicked () ), this, SLOT(startStopVideo() ) );
	connect(ui.btnPhoto,    SIGNAL(clicked () ), this, SLOT(savePhoto() ) );
	connect(&waitCamera,    SIGNAL(timeout () ), this, SLOT(getDeviceCapabilities() ) );
	connect(&keepZoomer,    SIGNAL(timeout () ), this, SLOT(keepZoomerTimeOut()) );
	connect(camera,    	SIGNAL(imageReady () ), this, SLOT(getImage() ));
	connect(ui.crButton,	SIGNAL(clicked () ), this, SLOT(crStartStop() ) );
	
	connect(ui.satManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( satChanged()) );
	connect(ui.freqBox,		SIGNAL(stateChanged (int) ), this, SLOT( freqChanged()) );
	connect(ui.brightManualValueBox,SIGNAL(valueChanged (int) ), this, SLOT( brightChanged()) );
	connect(ui.contManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( contChanged()) );
	connect(ui.sharpManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( sharpChanged()) );
	
	connect(ui.redSlider,   SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged() ));
	connect(ui.greenSlider, SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged() ));
	connect(ui.blueSlider,  SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged() ));
*/
}

KaptureWin::~KaptureWin()
{
	delete camera;
}

void KaptureWin::mError(int ret)
{
	switch (ret)
	{
		case 16:
			mfw->ui.mainFrameLabel->setText("Camera is already in use. You still can control the camera.");
			break;
		default :
			mfw->ui.mainFrameLabel->setText("An unknown error occured.");
			printf("An unknown error occured (%d)", ret);
			break;
	}
}

void KaptureWin::getDeviceCapabilities()
{
	int selected;
	if (!(result = camera->open(videoDevice.toLatin1().constData())))
	{
		if(waitCamera.isActive())
			waitCamera.stop();

		ui.comboBoxFormat->clear();
		ui.comboBoxSize->clear();

		formatList = camera->getFormatList(&formatName);
		for (int i = 0; i < formatList.size() ; i++)
			this->ui.comboBoxFormat->addItem(formatName.at(i));
	
		if(fctExecuted != 0)
			printf("\n");
		
		printf(" * Device is%s: %s\n", otherVideoDevice ? " [default] " : " ", videoDevice.toLatin1().constData());
		
		camera->setFormat(0, 0, formatList.at(0));
		QList<QSize> sizes = camera->getSizesList();
		bool modified = false;
		for (int i = 0; i < sizes.size(); i++)
		{
			sprintf(formatString, "%dx%d", sizes.at(i).width(), sizes.at(i).height() );
			this->ui.comboBoxSize->addItem(formatString);
			if (sizes.at(i).width() == 320 && sizes.at(i).height() == 240)
			{
				selected = i;
				camera->setFormat(320, 240, formatList.at(ui.comboBoxFormat->currentIndex()));
				modified = true;
			}
		}
		// I set the first frame size if the default size doesn't exist with the first format
		if(!modified)
		{
			changeSize(ui.comboBoxSize->itemText(0));
			ui.comboBoxSize->setCurrentIndex(0);

		}
		else
			ui.comboBoxSize->setCurrentIndex(selected);

		camera->getSizesList();

		ui.btnPhoto->setEnabled(true);
		ui.btnVideo->setEnabled(true);
		ui.comboBoxSize->setEnabled(true);
		
		/*
		 * It must be connected here because if it doesn't find any webcam at start, 
		 * it will change format in this function even if no camera is pluged in.
		 * 
		 */
		
		connect(ui.comboBoxSize, SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeSize(const QString &) ) );
		connect(ui.comboBoxFormat, SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeFormat(const QString &) ) );
	}
	else
	{
		if (fctExecuted == 0)
		{
			ui.comboBoxSize->clear();
			ui.btnPhoto->setEnabled(false);
			ui.btnVideo->setEnabled(false);
			ui.comboBoxSize->setEnabled(false);
			printf(" * No webcam pluged in (unable to open %s)\n", videoDevice.toLatin1().constData());
			printf(" * Waiting for you .");
			fflush(stdout);
			mfw->ui.mainFrameLabel->setText("No Webcam found !\n");
			waitCamera.start(2000);
			//TODO: This utility could use QDbus...
		}
		else
		{
			printf(".");
			fflush(stdout);
		}
	}
	fctExecuted++;
}

void KaptureWin::changeSize(const QString & itemSelected)
{
	/* 
	 * TODO:Review the structure of this function
	 */

	int ret = 0;
	const char *l_text;
	char checkCar;
	unsigned int newWidth = 0, newHeight = 0;
	l_text = itemSelected.toLatin1().constData();
	printf(" * Selected Format : %s\n", l_text);
	sscanf(l_text, "%d%c%d", &newWidth, &checkCar, &newHeight);
	if (newWidth > 320 && newHeight > 240)
	{
		if (crIsActivated)
			crStartStop();
		ui.crButton->setEnabled(false);
	}
	else
	{
		ui.crButton->setEnabled(true);
	}
	if (camera->isStreaming)
	{
		camera->stopStreaming();
		if(checkCar == 'x')
		{
			if ((ret = camera->setFormat(newWidth, newHeight, camera->currentPixelFormat())) == EXIT_FAILURE)
			{
				printf("***An error occured while setting format...Ret = %d\n", ret);
				mfw->ui.mainFrameLabel->setText("A problem occured while setting format. Check the device.");
				ui.btnVideo->setText("Start video");
				mError(ret);
			}
			else
			{
				mfw->ui.mainFrameLabel->resize(newWidth, newHeight);
				mfw->resize(newWidth, newHeight);
				ret = camera->startStreaming();
			}
		}
			
	}
	else
	{
		if(checkCar == 'x')
		{
			if ((ret = camera->setFormat(newWidth, newHeight, camera->currentPixelFormat())) == EXIT_FAILURE)
			{
				printf("***An error occured while setting format...Ret = %d\n", ret);
				mfw->ui.mainFrameLabel->setText("A problem occured while setting format. Check the device.");
				ui.btnVideo->setText("Start video");
			}

			else
			{
				mfw->ui.mainFrameLabel->resize(newWidth, newHeight);
				mfw->resize(newWidth, newHeight);
			}
		}
	}
}

void KaptureWin::changeFormat(const QString & itemSelected)
{
	ui.comboBoxSize->disconnect(SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeSize(const QString &) ));
	bool wasStreaming = false, modified = false;
	int lastWidth  = camera->currentWidth();
	int lastHeight = camera->currentHeight();

	ui.comboBoxSize->clear();
	printf(" * Changing format to %s\n", itemSelected.toLatin1().constData());
	if (camera->isStreaming)
	{
		startStopVideo();
		wasStreaming = true;
	}
/*
 * Is going to be replaced by the QList<QSize> getSizesList()
 *
	if (camera->setFormat(0, 0, formatList.at(el)) == EXIT_SUCCESS)
	{
		printf(" * Minimal format : %dx%d\n", camera->currentWidth(), camera->currentHeight());
		sprintf(formatString, "%dx%d (Minimal)", camera->currentWidth(), camera->currentHeight() );
		this->ui.comboBoxSize->addItem(formatString);
	}

	if (camera->setFormat(33000, 33000, formatList.at(el)) == EXIT_SUCCESS)
	{
		printf(" * Maximal format : %dx%d\n", camera->currentWidth(), camera->currentHeight());
		sprintf(formatString, "%dx%d (Maximal)", camera->currentWidth(), camera->currentHeight() );
		this->ui.comboBoxSize->addItem(formatString);
	}
	
	if (camera->setFormat(lastWidth, lastHeight, formatList.at(el)) == EXIT_SUCCESS)
	{
		printf(" * Reset format to : %dx%d\n", camera->currentWidth(), camera->currentHeight());
		sprintf(formatString, "%dx%d", camera->currentWidth(), camera->currentHeight() );
		this->ui.comboBoxSize->addItem(formatString);
	}
 */
	camera->setFormat(lastWidth, lastHeight, formatList.at(ui.comboBoxFormat->currentIndex()));
	QList<QSize> sizes = camera->getSizesList();
	for (int i = 0; i < sizes.size(); i++)
	{
		sprintf(formatString, "%dx%d", sizes.at(i).width(), sizes.at(i).height() );
		this->ui.comboBoxSize->addItem(formatString);
		if (sizes.at(i).width() == lastWidth && sizes.at(i).height() == lastHeight)
		{
			this->ui.comboBoxSize->setCurrentIndex(i);
			camera->setFormat(lastWidth, lastHeight, formatList.at(ui.comboBoxFormat->currentIndex()));
			modified = true;
		}
	}
	// I set the best little frame size if the last size doesn't still exists with the new format 
	if(!modified)
		changeSize(this->ui.comboBoxSize->itemText(0));
	
	if (wasStreaming)
		startStopVideo();
	connect(ui.comboBoxSize, SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeSize(const QString &) ) ); //Slot must be changeFormat()
	
}

void KaptureWin::startStopVideo()
{
	int ret = 0;
	if(camera->isStreaming)
	{
		if (camera->stopStreaming() == EXIT_SUCCESS)
		{
			ui.btnVideo->setText("Start video");
			keepZoomer.start(100);
		}
	}
	else
	{
		if (!camera->isOpened)
			camera->open(videoDevice.toLatin1().constData());

		ret = camera->startStreaming();
		if (ret == EXIT_FAILURE)
		{
			mfw->ui.mainFrameLabel->setText("An error occured while restarting the camera.");
			printf("***Unable to start streaming. Ret = %d\n", ret);
		}
		else
		{
			ui.btnVideo->setText("Stop video");
			keepZoomer.stop();
		}
	}
}

void KaptureWin::savePhoto()
{
	char fileName[50];
	int i = 0;
	FILE *file;
	sprintf(fileName,"%s%s%d%s", getenv("HOME"), "/Kapture-", i, ".jpg");
	while((file = fopen(fileName,"r")) != NULL)
	{
		fclose(file);
		i++;
		sprintf(fileName,"%s%s%d%s", getenv("HOME"), "/Kapture-", i, ".jpg");
	}
	if(imageFromCamera.save(fileName, 0, 100))
		printf(" * File \"%s\" successfull saved.\n", fileName);
	else
		printf("/*\\Saving \"%s\" failed\n", fileName);
}

void KaptureWin::getImage()
{
	QPixmap pixmap;
	if (camera->getFrame(&imageFromCamera) == EXIT_FAILURE)
	{
		startStopVideo();
		camera->close();
		fctExecuted = 0;
		ui.comboBoxSize->disconnect(SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeFormat(const QString &) ));
		waitCamera.start(2000);
		return;
	}
	
	if(!imageFromCamera.isNull())
	{
		pixmap = QPixmap::fromImage(imageFromCamera);
	}

	if(!pixmap.isNull())
	{
		mfw->ui.mainFrameLabel->setPixmap(pixmap);
		if(crIsActivated)
		{
			//Show the modified Picture.
			showColorReplaced();
		}
		showZoom();
		
	}
}

int KaptureWin::showZoom() // Shows little zoomed frame and shows the square in th normal frame
{
	
	if (camera->currentWidth() > 320 && camera->currentHeight() > 240)
		return -1;

	QImage mainImage;
	int i,j;
	int X = mfw->ui.mainFrameLabel->posCurX;
	int Y = mfw->ui.mainFrameLabel->posCurY;
	char pos[20];
	char rgbPixelText[30];
	

	// Repositioning the square in the frame.
	if(X >= mfw->ui.mainFrameLabel->width()-8)
		X = mfw->ui.mainFrameLabel->width()-9;
	if(Y >= mfw->ui.mainFrameLabel->height()-9)
		Y = mfw->ui.mainFrameLabel->height()-9;

	if(X <= 7)
		X = 8;
	if(Y <= 7)
		Y = 8;
	
	// Fill the image	
	if(camera->isStreaming)
	{
		mainImage = mfw->ui.mainFrameLabel->pixmap()->toImage();
		imageSaved = false;
	}
	
	if ((!camera->isStreaming) && (!imageSaved))
	{
		mainImage = imageFromCamera;
		mainImageSav = imageFromCamera;
		imageSaved = true;
	}
	if ((!camera->isStreaming) && (imageSaved))
	{
		mainImage = mainImageSav;
	}

	// Do zoom
	ui.zoomFrameLabel->setPixmap(zoomAtCursor(X, Y, mainImage));
	
	// Show Square
	for(i = (X-8); i <= (X + 8); i++)
	{
		for(j = (Y-8); j <= (Y + 8); j++)
		{
			if((i == (X-8)) || (i == (X+8)))
				mainImage.setPixel(i, j, qRgb(255, 0, 0));
			else
				if((j == (Y-8)) || (j == (Y+8)))
					mainImage.setPixel(i, j, qRgb(255, 0, 0));
			
		}
	}

	mfw->ui.mainFrameLabel->setPixmap(QPixmap::fromImage(mainImage));
	sprintf(pos, "Position : %dx%d", X, Y);
	ui.zoomPosLabel->setText(pos);
	sprintf(rgbPixelText, "R : %d, G : %d, B : %d",qRed(mainImage.pixel(X, Y)),
			qGreen(mainImage.pixel(X, Y)),
			qBlue(mainImage.pixel(X, Y)));
	ui.rgbPixelLabel->setText(rgbPixelText);
	if (!crIsActivated)
	{
		QPalette palette;
		palette.setColor(QPalette::Foreground, QColor ("black"));
		ui.rgbPixelLabel->setPalette(palette);
	}

	
	return 0;
}

QPixmap KaptureWin::zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn) // Return the little zoomed frame.
{
	if (camera->currentWidth() > 320 && camera->currentHeight() > 240)
		return QPixmap::QPixmap(60, 60);

	int X,Y,i=0,j=0;
	X = xCurPos;
	Y = yCurPos;
	if((X > 7) && (X < 313) && (Y > 7) && (Y < 233))
	{
		// We place the seeker at (0,0)
		X = X - 8;
		Y = Y - 8;
		
		for(; i<60; i++) // Lines
		{
			for(; j<60; j++) // Columns
			{
				if((j%4) == 0)
					X++;
				// FIXME: Would be better if it shows a littles rectangle.	
				if( (i>24) && (j>23) && (j<36) && (i<37) && (i!=29) && (i!=30) && (i!=31) && (i!=32) && (j!=28) && (j!=29) && (j!=30) && (j!=31) )
				{
					imageZoomed->setPixel(j, i, qRgb(0, 0, 0) );
				}
				else
				{
					imageZoomed->setPixel(j, i, imageToZoomIn.pixel(X,Y) );
				}

			}
			j = 0;
			X = xCurPos - 8; // x is reinitialized.
			if ((i%4) == 0)
				Y++;
		}
	}
	return QPixmap::fromImage(*imageZoomed);
}

void KaptureWin::crStartStop()
{
	if(crIsActivated)
	{
		crIsActivated = false;
		this->resize(this->width(), 120);
		ui.crBox->hide();
	}
	else
	{
		crIsActivated = true;
		this->resize(this->width(), 500);
		ui.crBox->show();
	}
}

void KaptureWin::colorChanged()
{
	QPixmap *color = new QPixmap(42, 42);
	color->fill(QColor(ui.redSlider->sliderPosition(), 
			   ui.greenSlider->sliderPosition(), 
			   ui.blueSlider->sliderPosition(),255 ));
	this->ui.colorShow->setPixmap(*color);
	ui.redSliderValue->display(ui.redSlider->sliderPosition() );
	ui.greenSliderValue->display(ui.greenSlider->sliderPosition() );
	ui.blueSliderValue->display(ui.blueSlider->sliderPosition() );
	color->~QPixmap();
	
}


void KaptureWin::showColorReplaced()
{
	int height;
	int width;
	struct levels {
		int red ;
		int green ;
		int blue ;
	} level;
	QImage image;
	QRgb qrgb;	
	int lwRedValue, lwGreenValue, lwBlueValue, lwRedSeverity, lwGreenSeverity, lwBlueSeverity;
	QPalette palette;
	
	if(camera->isStreaming)
		image = mfw->ui.mainFrameLabel->pixmap()->toImage();
	else
		image = imageFromCamera;

	crImage.fill(0);
	height = image.height();
	width  = image.width();
	
	lwRedValue = ui.redSlider->sliderPosition();
	lwGreenValue = ui.greenSlider->sliderPosition(); 
	lwBlueValue = ui.blueSlider->sliderPosition();
	lwRedSeverity = ui.redSeverityBox->value();
	lwGreenSeverity = ui.greenSeverityBox->value();
	lwBlueSeverity = ui.blueSeverityBox->value();
					
	palette.setColor(QPalette::Foreground, QColor ("red"));
	ui.rgbPixelLabel->setPalette(palette);
				
	int j = 0;
	for(int i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			qrgb = image.pixel(i, j);
			
			// Check the pixel
			if(((level.red = qRed(qrgb)) <= (lwRedValue + lwRedSeverity)) && 
					((level.green = qGreen(qrgb)) <= (lwGreenValue + lwGreenSeverity)) && 
					((level.blue = qBlue(qrgb)) <= (lwBlueValue + lwBlueSeverity)) && 
					((level.red = qRed(qrgb)) >= (lwRedValue - lwRedSeverity)) && 
					((level.green = qGreen(qrgb)) >= (lwGreenValue - lwGreenSeverity)) && 
					((level.blue = qBlue(qrgb)) >= (lwBlueValue - lwBlueSeverity)))
			{
				if((i < 319) &&  (image.pixel((i + 1), j) != qRgb(0, 0, 255)))
					crImage.setPixel((i + 1), j, qRgb(0, 0, 128));
				if((i > 0) &&  (image.pixel((i - 1), j) != qRgb(0, 0, 255)))
					crImage.setPixel((i - 1), j, qRgb(0, 0, 128));
				crImage.setPixel(i, j, qRgb(0,0,255));

				// Color the label showing RGB codes in green when it correspond.
				if ((mfw->ui.mainFrameLabel->posCurX == i) && (mfw->ui.mainFrameLabel->posCurY == j))
				{
					palette.setColor(QPalette::Foreground, QColor ("green"));
					ui.rgbPixelLabel->setPalette(palette);
				}
			}
		}
	}
	ui.crFrameLabel->setPixmap(QPixmap::fromImage(crImage));
}

void KaptureWin::keepZoomerTimeOut()
{
	showZoom();
	if(crIsActivated)
		showColorReplaced();
}

void KaptureWin::satChanged(/*int ctrl, int value*/)
{
	camera->changeCtrl(0, ui.satManualValueBox->value());
}

void KaptureWin::freqChanged()
{
	camera->changeCtrl(1, ui.freqBox->isChecked() ? 1 : 2);
}

void KaptureWin::brightChanged()
{
	camera->changeCtrl(2, ui.brightManualValueBox->value());
}

void KaptureWin::contChanged()
{
	camera->changeCtrl(3, ui.contManualValueBox->value());
}

void KaptureWin::sharpChanged()
{
	camera->changeCtrl(4, ui.sharpManualValueBox->value());
}

void KaptureWin::closeEvent(QCloseEvent *event)
{
	printf("\n * Exiting...\n");
	if (camera->isStreaming)
		startStopVideo();
	camera->close();
	((QApplication*) this->parentWidget())->quit();
}

void KaptureWin::setXmppUserName()
{
	printf(" * I set the username................\n");
}
