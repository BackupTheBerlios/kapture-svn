#ifndef XMLELEMENT_H
#define XMLELEMENT_H

#include <QtXml>
#include <QtGui>
#include <QtCore>
#include <QString>

class xmlElement
{
public:
	xmlElement();
	xmlElement(QString newTagName, QString newText, QXmlAttributes newAttributes);
	~xmlElement();
	
	void setTagName(QString newTagName);
	void setText(QString newText);
	void setAttributes(QXmlAttributes newAttributes);
	void setChild(xmlElement newChild);

private:
	QString tagName;
	QXmlAttributes attributes;
	QString text;
	QList<xmlElement> childs;
};
#endif
