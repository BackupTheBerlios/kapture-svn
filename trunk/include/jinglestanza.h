#ifndef JINGLESTANZA_H
#define JINGLESTANZA_H

#include "stanza.h"

class JingleStanza : public Stanza
{
	Q_OBJECT
public:
	JingleStanza();
	~JingleStanza();
	JingleStanza(const Stanza&);
	QString sid() const;
	bool isValid() const {return valid;};
	enum JingleStreamAction
	{
		SessionInitiate = 0,
		SessionTerminate,
		SessionAccept,
		SessionInfo,
		ContentAdd,
		ContentRemove,
		ContentModify,
		ContentReplace,
		ContentAccept,
		TransportInfo,
		NoAction
	};
	JingleStreamAction action() const;
	virtual QDomNode node() const {return n;};

private:
	QDomNode n; //Stanza tree.
	bool valid;
};

#endif
