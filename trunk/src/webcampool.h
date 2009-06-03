#ifndef WEBCAM_POOL_H
#define WEBCAM_POOL_H

#include <QList>

class Webcam;
class QComboBox;

class WebcamPool//: public QObject
{
	//Q_OBJECT
public:
	WebcamPool();
	~WebcamPool();

	/**
	 * Fills the given QComboBox with the devices name.
	 * Each item in the combo box will correspond to
	 * the index of the device that you give to device(int).
	 */
	void fillDeviceComboBox(QComboBox *c);

	/**
	 * returns the device at index i.
	 */
	Webcam* device(int i);
	int count() const;

	static WebcamPool* self();

private:
	QList<Webcam*> devices;
};

#endif
