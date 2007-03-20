#ifndef XMPP_H
#define XMPP_H

#include <QtCore>
#include <QtNetwork>

class xmpp
{
public: 
	xmpp(QString jid);
	~xmpp();
	int auth(char *password, char *ressource);
	bool connected();

private:	
	int sendData(QString mess);
	QByteArray readData();
	QTcpSocket *tcpSocket;
	/*
	 * See if char cannot be QString.
	 */
	QString server;
	QString username;
	int timeOut;
	struct rooster
	{
		QString subscription;
		QString name;
		QString jid;
		QString ask;
	};
	QList<rooster> roosterList;
	bool isConnected;
};

#endif //XMPP_H
