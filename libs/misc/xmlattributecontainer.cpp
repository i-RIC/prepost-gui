#include "xmlattributecontainer.h"

XmlAttributeContainer::XmlAttributeContainer()
{
	m_prefix = "";
}

XmlAttributeContainer::~XmlAttributeContainer()
{

}

const QString XmlAttributeContainer::attName(const QString& name) const
{
	if (m_prefix.isEmpty()){return name;}

	QString fullName = m_prefix;
	fullName.append(name.left(1).toUpper());
	fullName.append(name.mid(1));
	return fullName;
}
