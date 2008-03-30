#ifndef JID_H
#define JID_H

#include <QString>
#include <QStringList>

class Jid
{
public:
	Jid();
	Jid(const char *jid);
	Jid(const QString &jid);
	~Jid();

	//QString toString() const;
	//Jid& operator=(QString& jid);
	bool isValid() const;
	QString node() const;
	QString resource() const;
	QString domain() const;
	QString bare() const;
	QString full() const;
	
	void setNode(const QString &node);
	void setDomain(const QString &domain);
	void setResource(const QString &resource);

	bool equals(const Jid& other, bool withResource = false);
	bool operator==(Jid &other) const;
private:
	QString n; // node
	QString r; // resource
	QString d; // domain
	bool valid;
};

#endif //JID_H
