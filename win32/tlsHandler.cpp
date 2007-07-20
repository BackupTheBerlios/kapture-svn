#include "tlsHandler.h"

#include <iostream>
#include <QList>
#include <errno.h>

using namespace std;

TlsHandler::TlsHandler()
{
	SSL_library_init();
	SSL_load_error_strings();

	// Trying TLSv1 methode
	ctx = SSL_CTX_new( TLSv1_client_method() );
	ssl = SSL_new(ctx);
	rbio = BIO_new( BIO_s_mem() );
	wbio = BIO_new( BIO_s_mem() );
	SSL_set_bio(ssl, rbio, wbio);
	state = Connecting;
}

TlsHandler::~TlsHandler()
{

}

void TlsHandler::connect()
{
	//printf("TlsHandler::connect\n");
	update();
}

void TlsHandler::update()
{
	QByteArray buffer;
	unsigned int size;
	int ret, error;

	//printf("TlsHandler::update\n");

	switch(state)
	{
	case Connecting:
		ret = SSL_connect(ssl);

		if (ret < 0)
		{
			error = SSL_get_error(ssl, ret);
			if (error != SSL_ERROR_WANT_READ && error != SSL_ERROR_WANT_WRITE)
			{
				printf("ERROR : %d, ret = %d\n", error, ret);
				//emit error;
				//return;
			}
			break;
		}

		printf("Tls connected !\n");
		state = Connected;
		emit connected();
		/* Fall-through */

	case Connected:
		// Process clear data from SSL.
		QByteArray temp;
		bool eof = false;

		temp.resize(8192);
		while (!eof)
		{
                	ret = SSL_read(ssl, temp.data(), temp.size());
                	if (ret <= 0)
                	{
				//printf("error\n");
                    		eof = true;
                	}
                	else
			{
				unsigned int oldsize = xmlOut.size();
				xmlOut.resize(oldsize + ret);
				memcpy(xmlOut.data() + oldsize, temp.data(), ret);
			}
            	}
	
		// Process incoming clear data.
		if (!xmlIn.isEmpty())
		{
			ret = SSL_write(ssl, xmlIn.data(), xmlIn.size());
               		if (ret <= 0)
               		{
				//printf("error\n");
			}
			else
			{
				unsigned int newsize = xmlIn.size() - ret;
				memmove(xmlIn.data(), xmlIn.data()+ret, newsize);
				xmlIn.resize(newsize);
			}
		}
	
	}

	size = BIO_pending(wbio);
	if (size > 0) 
	{
		unsigned int oldsize = tlsOut.size();
		tlsOut.resize(oldsize + size);
		BIO_read(wbio, tlsOut.data() + oldsize, size);
	}
	
	if (!xmlOut.isEmpty())
	{
		//printf("TlsHandler : Clear data avaible to read by the application\n");
		emit readyRead(); // Clear data avaible to read by the application.
	}

	if (!tlsOut.isEmpty())
	{
		//printf("TlsHandler : Encrypted data avaible to be written on the socket\n");
		emit readyReadOutgoing(); // Encrypted data avaible to be written on the socket.
	}

}

void TlsHandler::write(const QByteArray& data)
{
	//printf("TlsHandler::write(%d bytes)\n", data.size());
	xmlIn.append(data);
	update();
}

void TlsHandler::writeIncoming(const QByteArray &data)
{
	//printf("TlsHandler::writeIncoming(%d bytes)\n", data.size());
	BIO_write(rbio, data.data(), data.size());
	update();
}

QByteArray TlsHandler::read()
{
	//printf("TlsHandler::read\n");
	QByteArray ret = xmlOut;
	xmlOut.clear();
	return ret;
}

QByteArray TlsHandler::readOutgoing()
{
	//printf("TlsHandler::readOutgoing\n");
	QByteArray ret = tlsOut;
	tlsOut.clear();
	return ret;
}

