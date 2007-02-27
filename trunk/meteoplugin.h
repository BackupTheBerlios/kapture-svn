#include <QPixmap>
#include <QImage>

QPixmap checkColor(QPixmap *pixmap, int lwRedValue, int lwGreenValue, int lwBlueValue, int lwRedSeverity, int lwGreenSeverity, int lwBlueSeverity);
QPixmap zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn);
