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
	needEmail = false;
	needPassword = false;
	needUsername = false;
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
	QDomNode node;
	switch (state)
	{
	case Start:
		if (s.type() != "result")
		{
			//emit error();
			return;
		}
		node = s.node().firstChild().firstChild();
		while (!node.isNull())
		{
			printf("[XMPPREG] node = %s\n", node.localName().toLatin1().constData());
			if (node.localName() == "username")
				needUsername = true;
			if (node.localName() == "password")
				needPassword = true;
			if (node.localName() == "email")
				needEmail = true;
			node = node.nextSibling();
		}
		sendRegistration();
		state = WaitResult;
		break;
	case WaitResult:
		if (s.type() == "result")
		{
			//registerOk = true;
			emit finished();
		}
		if (s.type() == "error")
		{
			;
			//registerOk = false;
			//emit error();
		}
		break;
	}
}

void XmppReg::sendRegistration()
{
	QString type = "set";
	id = randomString(6);
	Stanza stanza(Stanza::IQ, type, id, QString());
	QDomDocument doc("");
	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", "jabber:iq:register");
	
	if (needUsername)
	{
		QDomElement username = doc.createElement("username");
		QDomText text = doc.createTextNode(p.jid1().node());
		username.appendChild(text);
		query.appendChild(username);
	}
	
	if (needPassword)
	{
		QDomElement password = doc.createElement("password");
		QDomText text = doc.createTextNode(p.password());
		password.appendChild(text);
		query.appendChild(password);
	}
	
	if (needEmail)
	{
		QDomElement email = doc.createElement("email");
		QDomText text = doc.createTextNode(p.jid1().bare());
		email.appendChild(text);
		query.appendChild(email);
	}

	stanza.node().firstChild().appendChild(query);
	
	x->write(stanza);
	
}
