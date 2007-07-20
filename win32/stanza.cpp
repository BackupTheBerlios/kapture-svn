#include "stanza.h"


Stanza::Stanza()
{
	to = "";
	from = "";
	ns = "";
	type = "";
	message = "";
	subject = "";
	thread = "";
	priority = "";
	show = "";
	status = "";
}

void Stanza::setData(QByteArray n)
{
	to = "";
	from = "";
	ns = "";
	type = "";
	message = "";
	subject = "";
	thread = "";
	priority = "";
	show = "";
	status = "";
	
	QDomDocument doc;
	QByteArray dat = "<stanza>" + n + "</stanza>"; // A QDomDocument must have only one root element.
	doc.setContent(dat, true);
	QDomElement s = doc.documentElement();
	s = s.firstChildElement();

	while(!s.isNull())
	{
		kind = s.localName().toLower();
		if (kind != "iq" &&
		    kind != "message" &&
		    kind != "presence")
		{
			printf("This isn't a stanza : localName = %s\n", kind.toLatin1().constData());
			kind = "INVALID";
			return;
		}
		printf("Kind : %s\n", kind.toLower().toLatin1().constData());
		
		if (kind == "presence")
		{
			setupPresence(s);
			emit presenceReady();
		}
		if (kind == "message")
		{
			setupMessage(s);
			if (!message.isNull())
				emit messageReady();
			//else
			//	emit composing();
		}
		if (kind == "iq")
		{
			setupIq(s);
			emit iqReady();
		}
		s = s.nextSibling().toElement();
		printf("Tag = %s\n", s.localName().toLatin1().constData());
	}
}

Stanza::~Stanza()
{

}

void Stanza::setupPresence(QDomElement s)
{
	ns = s.attribute("xmlns");
	to = s.attribute("to");
	from = s.attribute("from");
	type = s.attribute("type", "available");
	printf("ns = %s\nto = %s\nfrom = %s\ntype = %s\n", ns.toLatin1().constData(), to.toLatin1().constData(), from.toLatin1().constData(), type.toLatin1().constData());

	printf("Changing the presence of %s\n", from.toLatin1().constData());

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");

	//printf("This node is %s\n", s.localName().toLatin1().constData());
	
	while (!s.isNull())
	{
		QString tag = s.localName();
	//	printf("tag = %s, XMLNS = '%s'\n", tag.toLatin1().constData(), s.attribute("xmlns", "NOXMLNS").toLatin1().constData());
		//if (s.hasAttribute("xmlns"))
			//printf("Has attributes\n");


		/* !!! DO NOT MODIFY s BEFORE ""s = s.nextSibling().toElement(); !!!*/
		
		
		if (tag == "priority")
		{
			if (s.firstChild().isText())
			{
				//printf("OK, it is text !\n");
				priority = s.firstChild().toText().data();
				printf("Priority =  %s\n", priority.toLatin1().constData());
			}
			else
			{
				printf("Error, no priority value.\n");
			}
		}
		
		if (tag == "show")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			show = s.firstChild().toText().data();
			printf("Show =  %s\n", show.toLatin1().constData());
		}
		if (tag == "status")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			status = s.firstChild().toText().data();
			printf("status =  %s\n", status.toLatin1().constData());
			
		}
		
		// This won't happend, I don't care now.
		if (tag == "x" && s.namespaceURI() == "vcard-temp:x:update") 
		{
			printf("vcard update !\n");
		}

		//printf("Next element ! \n");
		s = s.nextSibling().toElement();
	}

}

void Stanza::setupMessage(QDomElement s)
{
	to = s.attribute("to");
	from = s.attribute("from");
	type = s.attribute("type", "normal");

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");
	
		/* !!! DO NOT MODIFY s BEFORE ""s = s.nextSibling().toElement(); !!!*/
	while (!s.isNull())
	{
		QString tag = s.localName();
		if (tag == "subject")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			subject = s.firstChild().toText().data();
			printf("subject =  %s\n", subject.toLatin1().constData());
		}
		
		if (tag == "body")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			message = s.firstChild().toText().data();
			printf("message =  %s\n", message.toLatin1().constData());
		}
		
		if (tag == "thread")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			thread = s.firstChild().toText().data();
			printf("thread =  %s\n", thread.toLatin1().constData());
		}
		if (tag == "composing")
		{
			isComposing = true;
		}
		if (tag == "paused")
		{
			isComposing = false;
		}
		
		s = s.nextSibling().toElement();
	}
	printf("New message recieved from %s. (type = %s) :\n %s\n", from.toLatin1().constData(), type.toLatin1().constData(), message.toLatin1().constData());
	
	/*
	 * Xmpp will have to manage different types of message (chat, error, groupchat, headline, normal)
	 */
}

void Stanza::setupIq(QDomElement s)
{
	id = s.attribute("id");
	from = s.attribute("from");
	type = s.attribute("type");

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");
	
		/* !!! DO NOT MODIFY s BEFORE ""s = s.nextSibling().toElement(); !!!*/
	while (!s.isNull())
	{
		QString tag = s.localName();
		if (tag == "query")
		{
			if (id == "roster_1")
			{
				QDomNodeList items = s.childNodes();
				for (int i = 0; i < items.count(); i++)
				{
					contacts << items.at(i).toElement().attribute("jid");
					printf("New Roster contact : %s (subscription : %s)\n", contacts[i].toLatin1().constData(), items.at(i).toElement().attribute("subscription").toLatin1().constData());
				}
			}
/*
 <iq type='result' to='sender@jabber.org/resource' from='receiver@jabber.org/resource' id='info1'>
  <query xmlns='http://jabber.org/protocol/disco#info'>
    ...
    <feature var='http://jabber.org/protocol/si'/>
    <feature var='http://jabber.org/protocol/si/profile/file-transfer'/>
    ...
  </query>
</iq>
*/
			if (s.namespaceURI() == XMLNS_DISCO)
			{
				if (type == "get")
					action = SendDiscoInfo;
				if (type == "result")
				{
					QDomNodeList p = s.childNodes();
					
					for (int i = 0; i < p.count(); i++)
					{
						if (p.at(i).localName() == "feature")
							features << p.at(i).toElement().attribute("var");
					}

					action = ReceivedDiscoInfo;
				}
			}
			else
			{
				printf("Action : None\n");
				action = None;
			}
		}
		
		if (tag == "body")
		{
		}
		
		s = s.nextSibling().toElement();
	}
}

QStringList Stanza::getFeatures()
{
	QStringList f = features;
	features.clear();
	return f;
}

int Stanza::getAction()
{
	int a = action;
	action = None;
	return a;
}

QString Stanza::getFrom()
{
	return from;
}

QString Stanza::getTo()
{
	return to;
}

QString Stanza::getMessage()
{
	QString ret = message;
	message.clear();
	return ret;
}

QStringList Stanza::getContacts()
{
	QStringList ret = contacts;
	contacts.clear();
	return ret;
}

QString Stanza::getId()
{
	return id;
}

QString Stanza::getType()
{
	return type;
}


QString Stanza::getStatus()
{
	return status;
}

QString Stanza::getShow()
{
	return show;
}

void Stanza::setType(QString s)
{
	stan.setTagName(s);
	/*type = BadStanza;
	badStanzaStr = s;
	if (s == "iq" || s == "IQ")
		type = IQ;
	if (s == "message" || s == "MESSAGE")
		type = Message;
	if (s == "presence" || s == "PRESENCE")
		type = Presence;
	printf(" * Received Stanza : %s.\n", getQStringType().toLatin1().constData());
	*/
}

QByteArray Stanza::getData()
{
	/*if (type == IQ)
		return "iq";
	if (type == Message)
		return "message";
	if (type == Presence)
		return "presence";
	return badStanzaStr;*/
	return d.toString().toLatin1();
}
