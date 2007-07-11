#include "jid.h"

Jid::Jid(QString j)
{
	valid = true;
        //QString fromNode;
        if(j.split('/').count() == 0)
	{
		valid = false;
        }
        if(j.split('/').count() == 1)
	{
		node = j;
		resource = "";
	}
	if(j.split('/').count() == 2)
	{
		node = j.split('/').at(0);
		resource = j.split('/').at(1);
	}
}

Jid::~Jid()
{

}

QString Jid::toQString()
{
	if (resource != "")
	{
		return QString("%1/%2").arg(node, resource);
	}
	else
	{
		return node;
	}
}

bool Jid::isValid()
{
	return valid;
}

QString Jid::getNode()
{
	return node;
}

void Jid::setNode(QString n)
{
	node = n;
}

QString Jid::getResource()
{
	return resource;
}

void Jid::setResource(QString r)
{
	resource = r;
}

bool Jid::equals(Jid *other, bool withResource)
{
	if (!valid || !other->isValid())
		return false;
	if (withResource)
	{
		if (node == other->getNode() && resource == other->getResource())
			return true;
		else
			return false;
	}
	else
	{
		if (node == other->getNode())
			return true;
		else
			return false;
	}
}

bool Jid::operator==(Jid *other)
{
	if (!valid || !other->isValid())
		return false;
	
	if (resource == "" || other->getResource() == "")
	{
		if (node == other->getNode())
			return true;
		else
			return false;
	}
	else
	{
		if (node == other->getNode() && resource == other->getResource())
			return true;
		else
			return false;
		
	}
	return false;
}
