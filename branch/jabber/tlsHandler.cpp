#include "tlsHandler.h"
#include <QList>

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
	BIO_set_write_buf_size(wbio, 17408);
}

TlsHandler::~TlsHandler()
{

}

bool TlsHandler::connect()
{
	printf(" ! Unable to proceed (yet).\n");

	QByteArray buffer;
	state = Connecting;
	while ((ret = SSL_connect(ssl)) < 0)
	{
		error = SSL_get_error(ssl, ret);
		if (error == SSL_ERROR_WANT_WRITE)
		{
			printf(" * SSL want to write data !\n");
			return false;
		}
		
		if (error == SSL_ERROR_WANT_READ)
		{
			printf(" * Data avaible to read !\n");
			// Read from wbio
			bufSize = BIO_pending(wbio);
			printf("bufSize = %d\n", bufSize);
			buffer.resize(bufSize);
			BIO_read(wbio, buffer.data(), bufSize);
			printf("Writing : %d chars : '%s'\n", buffer.count() , buffer.data());
			//emit tlsDataAvaible(buffer.data(), bufSize);
			return false;
		}
		/*if (!tlsIn.isEmpty())
		{
			printf("write : %s", tlsIn[0].constData());
			BIO_puts(rbio, tlsIn.takeFirst().constData());
		}
		else
		{
			SSL_read(ssl, buffer, 1024);
			printf("buffer = %s\n", (char*) buffer);
		}
		return false;
		*/
	}
	

	return true;
}

void TlsHandler::setTlsIncoming(QByteArray &data)
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

