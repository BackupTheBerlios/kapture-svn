#include "webcampool.h"
#include "webcam.h"

#include <QComboBox>

WebcamPool *m_self = 0;

WebcamPool::WebcamPool()
{
	//TODO:should detect all webcams...
	Webcam *w = new Webcam();
	if (w->open("/dev/video0") == EXIT_SUCCESS)
		devices << w;
	w->close();
}

WebcamPool::~WebcamPool()
{

}

void WebcamPool::fillDeviceComboBox(QComboBox* c)
{
	for (int i = 0; i < devices.count(); ++i)
		c->addItem(devices.at(i)->name());
}

Webcam* WebcamPool::device(int i)
{
	if (i >= devices.count())
		i = devices.count() - 1;
	
	return devices.at(i);
}

int WebcamPool::count() const
{
	return devices.count();
}

WebcamPool *WebcamPool::self()
{
	if (m_self == 0)
		m_self = new WebcamPool();
	
	return m_self;
}
