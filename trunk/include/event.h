#ifndef EVENT_H
#define EVENT_H

#include <QString>
#include "stanza.h"

class Event
{
public:
	enum Type {
		Stream = 0,
		Stan,
		EndOfStream
	};

	Event(Type type, QDomNode node = QDomNode());
	~Event();

	int type() const { return t; };
	QDomNode node() const { return n; };

private:
	Type t;
	QDomNode n;
};

#endif //EVENT_H
