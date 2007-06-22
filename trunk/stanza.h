#ifndef STANZA_H
#define STANZA_H

#include <QString>
#include <QtXml>

class Stanza : public QObject
{
	Q_OBJECT
public:
	enum Kind {IQ, Message, Presence, BadStanza};
	Stanza();
	Stanza(QString kind, QString type, QString id);
	~Stanza();
	void setData(QByteArray node);
	void setType(QString s);
	//QString getQStringType(Kind kind);
	QByteArray getData();
	QString getFrom();
	QString getTo();
	QString getMessage();
	QStringList getContacts();
	QString getId();
	QString getType();
	QString getStatus();
	QString getShow();

signals:
	void presenceReady();
	void messageReady();
	void iqReady();

private:
	QString badStanzaStr;
	QString kind;
	QDomDocument d;
	QDomElement stan;
	void setupPresence(QDomElement s);
	void setupMessage(QDomElement s);
	void setupIq(QDomElement s);
	bool isComposing;

	QString to;
	QString id;
	QString from;
	QString ns;
	QString type;
	QString message;
	QString subject;
	QString thread;
	QString priority;
	QString show;
	QString status;
	QStringList contacts;

};

#endif //STANZA_H
