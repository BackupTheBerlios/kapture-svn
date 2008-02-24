#include "joystick.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*typedef uint32_t __u32;
typedef uint8_t __u32;

struct js_event
{
	__u32 time;     // event timestamp in milliseconds
	__s16 value;    // value
	__u8 type;      // event type
	__u8 number;    // axis/button number
};
*/

struct js_event
{
	uint32_t time;
	uint32_t data;
};

Joystick::Joystick()
{

}

Joystick::Joystick(const QString& devName)
{
	dev = open("/dev/js0", O_RDONLY);
	device = new QSocketNotifier(dev, QSocketNotifier::Read);
	connect(device, SIGNAL(activate(int)), SLOT(read()));
}

Joystick::~Joystick()
{

}

void Joystick::read()
{
	/*printf("[JOYSTICK] Joystick event :\n[JOYSTICK]  * time = %d\n", e.time);
	printf("[JOYSTICK]  * value = 0x%16x\n", e.value);
	printf("[JOYSTICK]  * type = %d\n", e.type);
	printf("[JOYSTICK]  * number = %d\n", e.number);*/
	//js_event e;
	//read (dev, (void)&e, sizeof(js_event));
	//printf("Data = \n%s\n", QByteArray::fromHex(QByteArray(e.data)).constData());
}
