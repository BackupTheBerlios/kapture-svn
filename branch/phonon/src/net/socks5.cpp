#include "socks5.h"
#include "jid.h"

#include <stdint.h>
#include <openssl/sha.h>

Socks5::Socks5(const QString& sid, const Jid& from, const Jid& to)
{
	state = InitServer;
	noAuthSupported = false;
	s = sid;
	f = from;
	t = to;
	printf("[SOCKS5] Create Socks5\n");
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
	//Check version.
	if (data.at(0) == 0x5)
	{
		printf("[SOCKS5] Ok, it's SOCKS5.\n");
	}
	else
	{
		//emit error(NotSocks5Error);
		return;
	}
	
	int val = 0;
	switch (state)
	{
	case InitServer :
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

		printf("[SOCKS5] Ok, that's good.\n");
		d.append((char)0x5);
		d.append((char)0x0);
		state = WaitRequestServer;
		break;
	case WaitRequestServer :
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
				printf("\n[SOCKS5] Received =   %s\n[SOCKS5] Calculated = %s\n", sha.toHex().constData(), result.toHex().constData());

				if (result == sha)
				{
					printf("[SOCKS5] The same !\n");
					d = data;
					d[1] = (char)0x00;

				}
				else
				{
					printf("[SOCKS5] Not the same !!!\n");
					//emit error();
				
				}
				break;
			}
			break;
		//case 2 : //BIND
		//case 3 : //UDP ASSOCIATE
		}
		break;
	case WaitMethodClient :
		if (data.at(1) == 0x00)
		{
			// Send first Request.
			d.append((char)0x5);
			d.append((char)0x1);
			d.append((char)0x0);
			d.append((char)0x3);
			QByteArray sha = sha1(QString("%1%2%3").arg(s).arg(f.full()).arg(t.full()));
			d.append(sha.count());
			for (int i = 0; i < sha.count(); i++)
				d.append(sha.at(i));
			d.append((char)0x0);
			d.append((char)0x0);
			state = WaitConnectionClient;
		}
		break;
	case WaitConnectionClient :
		emit established();
		return;
	}

	emit readyRead();
}

void Socks5::connect()
{
	//Hello Message
	d.append((char)0x5);
	d.append((char)0x1);
	d.append((char)0x0);
	state = WaitMethodClient;
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

