#ifndef TLSHANDLER_H
#define TLSHANDLER_H

#include <openssl/ssl.h>
#include <QByteArray>
#include <QList>

class TlsHandler
{
public:
	TlsHandler(int fd);
	~TlsHandler();
	bool connect();
	void setTlsIncoming(QByteArray data);

private:	
	int ret, error;
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *rbio;
	BIO *wbio;
	QList<QByteArray> tlsIn;
	enum tState
	{
		Connecting
	} state;

};

#endif //TLSHANDLER_H
