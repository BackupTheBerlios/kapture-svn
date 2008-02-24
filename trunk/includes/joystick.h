#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <QObject>
#include <QString>
#include <QSocketNotifier>

class Joystick : public QObject
{
	Q_OBJECT
public:
	Joystick();
	Joystick(const QString&);
	~Joystick();
	void setDeviceName(const QString&);

signals:
	void turnLeft();
	void turnRight();
	void up();
	void down();

private slots:
	void read();

private:
	QSocketNotifier *device;
	int dev;
};

#endif //JOYSTICK_H
