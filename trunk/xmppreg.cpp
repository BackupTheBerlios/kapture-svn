/*
 * FIXME:This Class should be a task.
 */

#include "xmppreg.h"

XmppReg::XmppReg(Task* parent, Xmpp* xmpp)
	:Task(parent)
{
	regOk = false;
	state = Start;
	x = xmpp;
}

XmppReg::~XmppReg()
{

}

void XmppReg::registerAccount(const QString& username, const QString& password)
{

}

void XmppReg::registerAccount(const Profile& profile)
{
	p = profile;
	connect(x, SIGNAL(registerReady()), SLOT(slotRegister()));
	x->prepareToRegister(p.jid1().domain());
}

void XmppReg::slotRegister()
{
/*
<iq type='get' id='reg1'>
 <query xmlns='jabber:iq:register'/>
</iq>
*/
	printf("[XMPPREG] slotRegister\n");
	QString type = "get";
	id = randomString(6);
	Stanza stanza(Stanza::IQ, type, id, QString());
	QDomDocument doc("");
	QDomElement c = doc.createElement("query");
	c.setAttribute("xmlns", "jabber:iq:register");
	stanza.node().firstChild().appendChild(c);
	
	x->write(stanza);
	
}

Profile XmppReg::profile() const
{
	return p;
}

bool XmppReg::canProcess(const Stanza& s) const
{
	if(s.id() == id && s.kind() == Stanza::IQ) //FIXME:check more.
	{
		return true;
	}
	return false;
}

void XmppReg::processStanza(const Stanza& s)
{
	switch (state)
	{
	case Start:
		QDomNode node = s.node().firstChild();
		printf("[XMPPREG] node = %s\n", node.localName().toLatin1().constData());
		break;
	}
}
