#include "tlsHandler.h"
#include <QList>

TlsHandler::TlsHandler(int fd)
{
	// Negotiate TLS.
	SSL_library_init();
	SSL_load_error_strings();

	// Trying TLSv1 methode
	ctx = SSL_CTX_new( TLSv1_client_method() );
	ssl = SSL_new(ctx);
	rbio = BIO_new( BIO_s_mem() );
	wbio = BIO_new( BIO_s_mem() );
	SSL_set_bio(ssl, rbio, wbio);
	//SSL_set_fd(ssl, fd);
}

TlsHandler::~TlsHandler()
{

}

bool TlsHandler::connect()
{
/*
 * With QList, I can remove the first element and the other ones are
 * automatically deplaced.
 * That's important to have a "FIFO" (First In, First Out) for incoming data.

	QList<int> list;
	int a;
	list << 1 << 2 << 3 << 4 << 5;
	a = list.takeFirst();
	printf(" * First was %d and now is = %d\n", a, list.at(0));
*/
	printf(" ! Unable to proceed (yet).\n");
	return false;
/*
	void *buffer;
	state = Connecting;
	do
	{
		ret = SSL_connect(ssl);
		error = SSL_get_error(ssl, ret);
		printf(" * SSL want to read or write data !\n");
		if (!tlsIn.isEmpty())
		{
			printf("write : %s", tlsIn.constData());
			BIO_puts(rbio, tlsIn.takeFirst.constData());
		}
		else
		{
			SSL_read(ssl, buffer, 1024);
			printf("buffer = %s\n", (char*) buffer);
		}
		return false;
	}
	while (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE);
*/
/*	
	if (error == SSL_ERROR_WANT_WRITE)
	{
		printf(" * SSL want to write data !\n");
		return false;
	}
*/
	return true;
}

void TlsHandler::setTlsIncoming(QByteArray data)
{
/*
	void *buffer;
	SSL_set_connect_state(ssl);
	do
	{
		switch (error)
		{
			case SSL_ERROR_WANT_READ: printf("SSL_write = %d\n", ret = SSL_write(ssl, data.constData(), data.length()));
						  break;
			case SSL_ERROR_WANT_WRITE: printf("SSL_read = %d\n", ret = SSL_read(ssl, buffer, 1024));
						  break;
		}

		if (ret <= 0)
		{
			error = SSL_get_error(ssl, ret);
			printf("Error = %d\n", error);
		}
	}
	while (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE);
	printf("BIO_puts = %d\n", ret = BIO_puts(wbio, data.constData()));
*/
//	tlsIn.append(data); ???
}

