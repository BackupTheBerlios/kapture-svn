#include "jinglestanza.h"

JingleStanza::JingleStanza()
{
	valid = false;
}

JingleStanza::~JingleStanza()
{

}

JingleStanza::JingleStanza(const Stanza& s)
{
	if (!s.isJingle())
	{
		printf("[JingleStanza] NOT VALID !\n");
		valid = false;
	}
	else
	{
		n = s.node();
		valid = true;
	}
}

QString JingleStanza::sid() const
{
	return n.firstChildElement().attribute("sid");
}

JingleStanza::JingleStreamAction JingleStanza::action() const
{
	if (n.firstChildElement().attribute("action") == "session-initiate")
		return SessionInitiate;
	if (n.firstChildElement().attribute("action") == "session-terminate")
		return SessionTerminate;
	if (n.firstChildElement().attribute("action") == "session-accept")
		return SessionAccept;
	if (n.firstChildElement().attribute("action") == "session-info")
		return SessionInfo;

	if (n.firstChildElement().attribute("action") == "content-add")
		return ContentAdd;
	if (n.firstChildElement().attribute("action") == "content-remove")
		return ContentRemove;
	if (n.firstChildElement().attribute("action") == "content-modify")
		return ContentModify;
	if (n.firstChildElement().attribute("action") == "content-replace")
		return ContentReplace;
	if (n.firstChildElement().attribute("action") == "content-accept")
		return ContentAccept;
	
	if (n.firstChildElement().attribute("action") == "transport-info")
		return TransportInfo;
	
	return NoAction;
}

/*TODO: Also add the possibility to create Jingle Stanza.*/
