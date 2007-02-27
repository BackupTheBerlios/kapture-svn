/*
 *      meteoplugin.h -- Kapture
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

/*
 * This file will be soon removed.
 */

#include <QPixmap>
#include <QImage>

QPixmap checkColor(QPixmap *pixmap, int lwRedValue, int lwGreenValue, int lwBlueValue, int lwRedSeverity, int lwGreenSeverity, int lwBlueSeverity);
QPixmap zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn);
