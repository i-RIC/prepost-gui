#include "xmlattributecontainer.h"
#include "private/xmlattributecontainer_impl.h"

#include <QString>

XmlAttributeContainer::XmlAttributeContainer() :
	m_impl {new Impl {}}
{}

XmlAttributeContainer::XmlAttributeContainer(const XmlAttributeContainer& c) :
	XmlAttributeContainer {}
{
	*(m_impl) = *(c.m_impl);
}

XmlAttributeContainer::~XmlAttributeContainer()
{
	delete m_impl;
}

XmlAttributeContainer& XmlAttributeContainer::operator=(const XmlAttributeContainer& c)
{
	*(m_impl) = *(c.m_impl);
	copyValue(c);
	return *this;
}

const QString& XmlAttributeContainer::prefix() const
{
	return m_impl->m_prefix;
}

void XmlAttributeContainer::setPrefix(const QString& prefix)
{
	m_impl->m_prefix = prefix;
}

QString XmlAttributeContainer::attName(const QString& name) const
{
	if (m_impl->m_prefix.isEmpty()) {return name;}

	QString fullName = m_impl->m_prefix;
	fullName.append(name.left(1).toUpper());
	fullName.append(name.mid(1));
	return fullName;
}

void XmlAttributeContainer::copyValue(const XmlAttributeContainer&)
{}
