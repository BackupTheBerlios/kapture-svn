/*
 *      meteoplugin.cpp -- Kapture
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

#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QRgb>

#include "webcam.h"
#include "kapturewin.h"
#include "meteoplugin.h"

QImage *image = new QImage();
QImage *imageOut = new QImage(320, 240, QImage::Format_RGB32);
QRgb qrgb;
int height;
int width;
struct levels {
	int red ;
	int green ;
	int blue ;
} level;

QImage *imageZoom = new QImage(60, 60, QImage::Format_RGB32); // 4:1

QPixmap checkColor(QPixmap *pixmap, int lwRedValue, int lwGreenValue, int lwBlueValue, int lwRedSeverity, int lwGreenSeverity, int lwBlueSeverity)
{
	*image = pixmap->toImage();
	//image->create(pixmap->width(), pixmap->height(), QImage::Format_RGB32);
	imageOut->fill(0);
	height = image->height();
	width  = image->width();
//	printf("Depth = %d\n", image->depth());

	int j = 0;
	for(int i = 0; i < width; i++)
	{
		for(j = 0; j < height; j++)
		{
			qrgb = image->pixel(i, j);
			
			// Check the pixel
			if(((level.red = qRed(qrgb)) <= (lwRedValue + lwRedSeverity)) && 
					((level.green = qGreen(qrgb)) <= (lwGreenValue + lwGreenSeverity)) && 
					((level.blue = qBlue(qrgb)) <= (lwBlueValue + lwBlueSeverity)) && 
					((level.red = qRed(qrgb)) >= (lwRedValue - lwRedSeverity)) && 
					((level.green = qGreen(qrgb)) >= (lwGreenValue - lwGreenSeverity)) && 
					((level.blue = qBlue(qrgb)) >= (lwBlueValue - lwBlueSeverity)))
			{
				if((i < 319) &&  (image->pixel((i + 1), j) != qRgb(0, 0, 255)))
					imageOut->setPixel((i + 1), j, qRgb(0, 0, 128));
				if((i > 0) &&  (image->pixel((i - 1), j) != qRgb(0, 0, 255)))
					imageOut->setPixel((i - 1), j, qRgb(0, 0, 128));
				imageOut->setPixel(i, j, qRgb(0,0,255));
			}
			image->setPixel(i, j, qRgb(0,0,0));
		}
	}
	pixmap->fromImage(*image);
	return QPixmap::fromImage(*imageOut);
}

QPixmap zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn) // Return the littel zoomed frame.
{
	int x,y,i=0,j=0;
	x = xCurPos;
	y = yCurPos;
	if((x > 7) && (x < 313) && (y > 7) && (y < 233))
	{
		// We place the cursor at (0,0)
		x = x - 7;
		y = y - 7;
		
		for(; i<60; i++) // Lines
		{
			for(; j<60; j++) // Columns
			{
				if((j%4) == 0)
					x++;
				
				if( (i>28) && (i<33) && (j>28) && (j<33) )
				{
					imageZoom->setPixel(j, i, qRgb(0, 0, 0) );
				}
				else
				{
					imageZoom->setPixel(j, i, imageToZoomIn.pixel(x,y) );
				}

			}
			j = 0;
			x = xCurPos - 7; // x is reinitialized.
			if ((i%4) == 0)
				y++;
		}
	}
	return QPixmap::fromImage(*imageZoom);
}
