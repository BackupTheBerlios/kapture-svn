#ifndef STANZA_H
#define STANZA_H

#include <QString>
#include <QtXml>
//#define XMLNS_DISCO "http://jabber.org/protocol/disco#info"
#include "jid.h"

class Stanza : public QObject
{
	Q_OBJECT
public:
	enum Kind {IQ = 0, Message, Presence, BadStanza};
	/*!
	 * Create a new invalid Stanza.
	 */
	Stanza();
	/*!
	 * Create a new Stanza with kind Kind, type Type, id Id and to To.
	 */
	Stanza(Kind kind, const QString& type, const QString& id, const QString& to);
	/*!
	 * Create a new Stanza from an xml tree.
	 */
	Stanza(QByteArray &node);
	/*!
	 * Destructor.
	 */
	~Stanza();
	
	Kind tagNameToKind(QString tagName) const;
	QString kindToTagName(Kind kind) const;
	
	void setType(QString &s);
	void setFrom(const Jid&);
	void setTo(const Jid&);
	void setId(const QString&);
	void setType(const QString&);
	void setKind(Kind);
	void appendChild(const QDomNode&);

	Jid from() const;
	Jid to() const;
	QString id() const;
	QString type() const;
	Kind kind() const;
	QByteArray data() const;
	QDomDocument document() const;

private:
	/*!
	 * Stanza's xml tree.
	 */
	QByteArray n; // node
	Kind k; // kind
	Jid t; // to
	QString i; // id
	Jid f; // from
	QString ty; // type
	QDomDocument doc; // Xml Tree.

};

#endif //STANZA_H
