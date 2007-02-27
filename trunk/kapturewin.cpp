#include <unistd.h>
#include <QCheckBox>
#include <QAbstractButton>
#include <QImageReader>

#include "kapturewin.h"
#include "mainframewin.h"
#include "webcam.h"

/*
 * This Software is at a developpement state.
 * This is under GNU Public License.
 * You can use, modify or redistribute it.
 *
 * FIXME:
 * 	- Improve frame rate (slower after resizing....) [seems to be corrected]
 * 	- [...]
 *
 * TODO:
 * 	- Implement support for YUV.
 * 	- Add more goodies (B&W, ...)
 * 	- Add support for more controls (Pan-tilts,...)
 * 	- Add comments in the code.
 * 	- The timer can be replaced by Dbus (Qt >= 4.2)
 */

int result = 0;
char formatString[128] = "00000x00000 (MINIMAL)"; // FIXME: use a QString()
QImage *imageZoomed = new QImage(60,60,QImage::Format_RGB32); 


KaptureWin::KaptureWin()
	: QMainWindow()
{
	int i = 0;
	while(i < qApp->arguments().size())
	{
		printf("Element %i = %s\n", i, qApp->arguments().at(i).toLatin1().constData());
		i++;
	}
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
//	connect(ui.treatBtn,	SIGNAL(clicked () ), this, SLOT(treatFrame() ) );
	
	connect(ui.satManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( satChanged()) );
	connect(ui.freqBox,		SIGNAL(stateChanged (int) ), this, SLOT( freqChanged()) );
	connect(ui.brightManualValueBox,SIGNAL(valueChanged (int) ), this, SLOT( brightChanged()) );
	connect(ui.contManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( contChanged()) );
	connect(ui.sharpManualValueBox,	SIGNAL(valueChanged (int) ), this, SLOT( sharpChanged()) );
	
	connect(ui.redSlider,   SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged(/*int*/) ));
	connect(ui.greenSlider, SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged(/*int*/) ));
	connect(ui.blueSlider,  SIGNAL(sliderMoved (int) ), this, SLOT(colorChanged(/*int*/) ));

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
	if (!(result = camera->open("/dev/video0")))
	{
		if(waitCamera.isActive())
			waitCamera.stop();

	
		if(fctExecuted != 0)
			printf("\n");
		
		printf(" * Device is [default] : /dev/video0\n");
		
		if(camera->setFormat(0, 0) == EXIT_SUCCESS)
		{
			printf(" * Minimal format : %dx%d\n", camera->currentWidth(), camera->currentHeight());
			sprintf(formatString, "%dx%d (Minimal)", camera->currentWidth(), camera->currentHeight() );
			this->ui.comboBoxSize->addItem(formatString);
		}
		else
		{
			printf("Error while detecting the device. Retrying...\n");
			camera->close();
			waitCamera.start(2000);
			return;
		}

		if (camera->setFormat(33000, 33000) == EXIT_SUCCESS)
		{
			printf(" * Maximal format : %dx%d\n", camera->currentWidth(), camera->currentHeight());
			sprintf(formatString, "%dx%d (Maximal)", camera->currentWidth(), camera->currentHeight() );
			this->ui.comboBoxSize->addItem(formatString);
		}
		else
		{
			printf("Error while detecting the device. Retrying...\n");
			camera->close();
			waitCamera.start(2000);
			return;
		}
		
		if (camera->setFormat(320, 240) == EXIT_SUCCESS)
		{
			printf(" * Set Default format to : %dx%d\n", camera->currentWidth(), camera->currentHeight());
			sprintf(formatString, "%dx%d", camera->currentWidth(), camera->currentHeight() );
			this->ui.comboBoxSize->addItem(formatString);
		}
		else
		{
			printf("Error while detecting the device. Retrying...\n");
			camera->close();
			waitCamera.start(2000);
			return;
		}

		ui.comboBoxSize->setCurrentIndex( ui.comboBoxSize->count()-1 );
					
		ui.btnPhoto->setEnabled(true);
		ui.btnVideo->setEnabled(true);
		ui.comboBoxSize->setEnabled(true);
		
		/*
		 * It must be connected here because if it doesn't find any webcam at start, 
		 * it will change format in this function even if no camera is pluged in.
		 * 
		 */
		
		connect(ui.comboBoxSize, SIGNAL( currentIndexChanged(const QString &) ), this, SLOT(changeFormat(const QString &) ) );
	}
	else
	{
		if (fctExecuted == 0)
		{
			ui.comboBoxSize->clear();
			ui.btnPhoto->setEnabled(false);
			ui.btnVideo->setEnabled(false);
			ui.comboBoxSize->setEnabled(false);
			printf(" * No webcam pluged in (unable to open /dev/video0)\n");
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

void KaptureWin::changeFormat(const QString & itemSelected)
{
	/* 
	 * TODO:Review the structure of this function
	 */

	int ret = 0;
	const char *l_text;
	char checkCar;
	unsigned int newWidth = 0, newHeight = 0;
	l_text = itemSelected.toAscii().constData();
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
			if ((ret = camera->setFormat(newWidth, newHeight)) == EXIT_FAILURE)
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
			if ((ret = camera->setFormat(newWidth, newHeight)) == EXIT_FAILURE)
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
			camera->open("/dev/video0");

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

void KaptureWin::treatFrame()
{
	//Support for this goodie to implement
	int i=0, j=0, a=1;
	int height = crImage.height();
	int width  = crImage.width();
	
	for (i = 0 ; i < width; i++)
	{
		for (j = 0 ; j < height; j++)
		{
			if (qRgb(0, 0, 128) == crImage.pixel(i,j))
			{
				printf(" * Found a blue Pixel at %dx%d\n", i,j);
				crImage.setPixel(i,j,qRgb(0, 255, 0));
				if (j+a < height)
				{
					while (crImage.pixel(i,j+a) == qRgb(0, 0, 255))
					{
						if (j+a < height)
							crImage.setPixel(i,j+a,qRgb(0, 255, 0));
						printf(" * Found a blue pixel at %dx%d (2)\n", i, j+a);
						a++;
					}
				}
				else if (j+a == height)
				{
					while (crImage.pixel(i,j-a) == qRgb(0, 0, 255))
					{
						if (j-a > 0)
							crImage.setPixel(i,j+a,qRgb(0, 255, 0));
						printf(" * Found a blue pixel at %dx%d (2)\n", i, j+a);
						a++;
					}
				}
			}
		}
	}
//	ui.foundFrameLabel->setPixmap(QPixmap::fromImage(crImage));
	
	/*
	 * Has to treat the crImage : 
	 * 	
	 * 	* check for lines in the crImage
	 * 	* draw the processus of search in foundFrameLabel
	 * 	* result the coordonées of the start and end of lines
	 * 	
	 */ 
}

void KaptureWin::closeEvent(QCloseEvent *event)
{
	printf(" * Exiting...\n");
	if (camera->isStreaming)
		startStopVideo();
	camera->close();
	((QApplication*) this->parentWidget())->quit();
}

