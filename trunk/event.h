#ifndef EVENT_H
#define EVENT_H

#include <QString>

class Event
{
public:
	Event();
	~Event();
	QString localName;
	enum Type {Stream, Stanza, EndOfStream} type;
};

#endif //EVENT_H
