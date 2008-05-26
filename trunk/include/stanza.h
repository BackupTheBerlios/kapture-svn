#ifndef STANZA_H
#define STANZA_H

#include <QString>
#include <QtXml>

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
	Stanza(Kind kind, const QString& type, const QString& id, const QString& to, const QString& namespaceURI = QString());
	/*!
	 * Create a new Stanza from an xml node.
	 */
	Stanza(const QDomNode &node);
	/*!
	 * Destructor.
	 */
	~Stanza();
	
	Kind kind() const;
	Jid from() const;
	Jid to() const;
	QString id() const;
	QString type() const;
	QString namespaceURI() const;
	virtual QDomNode node() const;
	bool isJingle() const;

	void setKind(Kind);
	void setType(QString &s);
	void setFrom(const Jid&);
	void setTo(const Jid&);
	void setId(const QString&);
	void setType(const QString&);
	void setNode(const QDomNode&);

private:
	Kind tagNameToKind(QString tagName) const;
	QString kindToTagName(Kind kind) const;

	QDomNode n; // Xml Tree.

};

#endif //STANZA_H
