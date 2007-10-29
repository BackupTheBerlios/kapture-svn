#ifndef SOCKS5_H
#define SOCKS5_H

#include <QObject>
#include <QByteArray>
#include "jid.h"

class Socks5 : public QObject
{
	Q_OBJECT
public :
	Socks5(const QString& sid, const Jid& from, const Jid& to);
	~Socks5();
	void write(const QByteArray&);
	QByteArray read() const;
	void connect();

signals :
	void readyRead();
	void established();

private :
	QByteArray sha1(const QString& clear);
	enum State {InitClient = 0, WaitRequestClient, WaitMethod, WaitConnection} state;
	QByteArray d;
	void process(const QByteArray&);
	bool noAuthSupported;
	QString s; //SID.
	QByteArray sha;
	Jid f; //Initiator.
	Jid t; //Target.
};

#endif //SOCKS5_H
