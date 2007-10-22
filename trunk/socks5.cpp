#include "socks5.h"
#include "jid.h"

#include <stdint.h>
#include <openssl/sha.h>

Socks5::Socks5(const QString& sid, const Jid& from, const Jid& to)
{
	state = Init;
	noAuthSupported = false;
	s = sid;
	f = from;
	t = to;
}

Socks5::~Socks5()
{

}

void Socks5::write(const QByteArray& data)
{
	process(data);
}

QByteArray Socks5::read() const
{
	return d;
}

void Socks5::process(const QByteArray& data)
{
	d.clear();
	if (data.size() < 3)
		return;
	//Check version.
	if (data.at(0) == 0x5)
	{
		printf("Ok, it's SOCKS5.\n");
	}
	else
	{
		//emit error(NotSocks5Error);
		return;
	}
	
	int val = 0;
	switch (state)
	{
	case Init :
		//Number of supported METHODS.
		val = (int)data.at(1);
		for (int i = 0; i < val; i++)
		{
			if (data.at(2 + i) == 0x0)
			{
				noAuthSupported = true;
				break;
			}
		}
		if (!noAuthSupported)
			return;

		printf("Ok, that's good.\n");
		d.append((char)0x5);
		d.append((char)0x0);
		state = WaitRequest;
		break;
	case WaitRequest :
		switch (data.at(1))
		{
		case 1 : //CONNECT
			switch(data.at(3))
			{
			case 3 : //DOMAIN NAME
				QByteArray result  = sha1(QString("%1%2%3").arg(s).arg(f.full()).arg(t.full()));
				for (int i = 5; i <= (int)data.at(4) + 4; i++)
				{
					printf("0x%02x ", data.at(i));
					sha.append((char)data.at(i));
				}
				sha.resize((int)data.at(4));
				printf("\nReceived =   %s\nCalculated = %s\n", sha.toHex().constData(), result.toHex().constData());

				if (result == sha)
				{
					printf("The same !\n");
					d = data;
					d[1] = (char)0x00;

				}
				else
				{
					printf("Not the same !!!\n");
					//emit error();
				
				}
				break;
			}
			break;
		//case 2 : //BIND
		//case 3 : //UDP ASSOCIATE
		}
		break;
	}

	emit readyRead();
}

QByteArray Socks5::sha1(const QString& clear)
{
	const char *text = clear.toLatin1().constData();
	unsigned char sha1_encoded[20];

	SHA1((const unsigned char*)text, strlen(text), sha1_encoded);
	QByteArray ret = QByteArray((const char*)sha1_encoded, 20);
	ret = ret.toHex();
	ret.resize(40);
		
	return ret;
}

