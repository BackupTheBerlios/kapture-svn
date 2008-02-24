#ifndef TLSHANDLER_H
#define TLSHANDLER_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <QByteArray>
#include <QList>
#include <QtCore>

class TlsHandler: public QObject
{
/*
 *  - readyReadOutGoing is emitted when TlsHandler has data ready to be read 
 *    AND to be sent to the server.
 */
	Q_OBJECT
public:
	TlsHandler();
	~TlsHandler();
	void update();
	void connect();
	void writeIncoming(const QByteArray &data);
	void write(const QByteArray& data);
	QByteArray read();
	QByteArray readOutgoing();

private:	
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *rbio;
	BIO *wbio;
	QByteArray tlsOut;
	QByteArray xmlIn;
	QByteArray xmlOut;
	enum State 
	{
		Connecting,
		Connected,
	} state;
signals:
	void readyReadOutgoing();
	void readyRead();
	void connected();

};

#endif //TLSHANDLER_H
