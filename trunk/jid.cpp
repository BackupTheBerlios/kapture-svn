#include "jid.h"


Jid::Jid()
{

}

Jid::Jid(const char *j)
{

}

Jid::Jid(const QString& j)
{
	if (j.isEmpty())
	{
		valid = false;
		return;
	}

	valid = true;
        
	if(j.split('/').count() == 0)
	{
		valid = false;
        }
        
	if(j.split('/').count() == 1)
	{
		n = j.split('@').at(0);
		d = j.split('@').at(1);
		// FIXME:Must manage errors
		r = "";
		printf("n = %s, d = %s, r = %s\n", n.toLatin1().constData(), d.toLatin1().constData(), r.toLatin1().constData());
	}
	
	if(j.split('/').count() == 2)
	{
		n = j.split('@').at(0);
		d = j.split('@').at(1).split('/').at(0);
		r = j.split('/').at(1);
	}
}

Jid::~Jid()
{

}

QString Jid::full() const
{
	if (!valid)
		return QString();

	if (r != "")
	{
		return QString("%1@%2/%3").arg(n, d, r);
	}
	else
	{
		return QString("%1@%2").arg(n, d);
	}
}

bool Jid::isValid() const
{
	return valid;
}

QString Jid::node() const
{
	return n;
}

void Jid::setNode(const QString &node)
{
	n = node;
}

QString Jid::resource() const
{
	return r;
}

void Jid::setResource(const QString &resource)
{
	r = resource;
}

bool Jid::equals(const Jid& other, bool withResource)
{
	if (!valid || !other.isValid())
		return false;
	if (withResource)
	{
		if (n == other.node() && r == other.resource())
			return true;
		else
			return false;
	}
	else
	{
		if (n == other.node())
			return true;
		else
			return false;
	}
}

bool Jid::operator==(Jid &other) const
{
	if (!valid || !other.isValid())
		return false;
	
	if (r == "" || other.resource() == "")
	{
		if (n == other.node())
			return true;
		else
			return false;
	}
	else
	{
		if (n == other.node() && r == other.resource())
			return true;
		else
			return false;
		
	}
	return false;
}

QString Jid::domain() const
{
	return d;
}

QString Jid::bare() const // node@domain/resource --> bare = node@domain. Replace node !!!
{
	return n + '@' + d;
}

/*Jid & Jid::operator=(QString& jid)
{
	return Jid(jid);
}*/
