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
		for(int i = 0; i < data.count(); i++)
			printf("0x%x ", data.at(i));
		printf("\n");
		switch (data.at(1))
		{
		case 1 : //CONNECT
			printf("Asking to connect trough TCP/IP ");
			//pos = 3; // Position 2 is reserved.
			switch(data.at(3))
			{
			case 3 : //DOMAIN NAME
				printf("and giving a domain name of %d characters :\n", data.at(4));
				for(int i = 5; i <= data.at(4) + 4; i++)
					printf("%c", data.at(i));
				printf("\n");
				printf("Port = 0x%x 0x%x\n", data.at(data.count() - 2), data.at(data.count() - 1));
				
				QByteArray result  = sha1(QString("%1%2%3").arg(s).arg(f.full()).arg(t.full()));
				QByteArray result2 = QByteArray::fromHex(data);
				
				for (int i = 0; i < 20; i++)
					printf("%02x", (unsigned char) result2[i]); // FIXME:a part is missing.
				printf("\n");
				if (result == result2)
				{
					//SOCKS V5
					d.append((char)0x5);
					//SUCCESS
					d.append((char)0x0);
					//RESERVED
					d.append((char)0x0);
					//ATYP
					d.append((char)0x3);
					//BND.ADDR
					d.append(result.toHex());
					//BND.PORT
					d.append((char)0x0);
					d.append((char)0x0);

				}
				else
				{
					//SOCKS V5
					d.append((char)0x5);
					//FAILURE
					d.append((char)0x5);
					//RESERVED
					d.append((char)0x0);
					//ATYP
					d.append((char)0x3);
					//BND.ADDR
					d.append(result.toHex());
					//BND.PORT
					d.append((char)0x0);
					d.append((char)0x0);
				
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
	for (int i = 0; i < 20; i++)
		printf("%02x", sha1_encoded[i]); // FIXME:a part is missing.
	printf("\n");
		
	return QByteArray((const char*)sha1_encoded);
}

