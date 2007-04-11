#ifndef TLSHANDLER_H
#define TLSHANDLER_H

#include <openssl/ssl.h>
#include <QByteArray>
#include <QList>
#include <QtCore>

class TlsHandler: public QObject
{
	Q_OBJECT
public:
	TlsHandler();
	~TlsHandler();
	bool connect();
	void setTlsIncoming(QByteArray &data);
	QList<void*> toSend;
	int bufSize;

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
signals:
	void tlsDataAvaible(void *buffer, int bufSize);

};

#endif //TLSHANDLER_H
