#include "xmlElement.h"

xmlElement::xmlElement()
{

}

xmlElement::xmlElement(QString newTagName, QString newText, QXmlAttributes newAttributes)
{
	tagName = newTagName;
	text = newText;
	attributes = newAttributes;
}

xmlElement::~xmlElement()
{

}

void xmlElement::setTagName(QString newTagName)
{
	tagName = newTagName;
}

void xmlElement::setText(QString newText)
{
	text = newText;
}

void xmlElement::setAttributes(QXmlAttributes newAttributes)
{
	attributes = newAttributes;
}

void xmlElement::setChild(xmlElement newChild)
{
	childs.append(newChild);
}
