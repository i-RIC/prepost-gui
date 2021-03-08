#include "xmlattributecontainer.h"
#include "private/xmlattributecontainer_impl.h"

#include <misc/stringtool.h>

#include <QString>

#include <string>

XmlAttributeContainer::XmlAttributeContainer() :
	impl {new Impl {}}
{}

XmlAttributeContainer::XmlAttributeContainer(const XmlAttributeContainer& c) :
	XmlAttributeContainer {}
{
	*(impl) = *(c.impl);
}

XmlAttributeContainer::~XmlAttributeContainer()
{
	delete impl;
}

XmlAttributeContainer& XmlAttributeContainer::operator=(const XmlAttributeContainer& c)
{
	*(impl) = *(c.impl);
	copyValue(c);
	return *this;
}

const QString& XmlAttributeContainer::prefix() const
{
	return impl->m_prefix;
}

void XmlAttributeContainer::addPrefix(const QString& prefix)
{
	if (impl->m_prefix.isEmpty()) {
		impl->m_prefix = prefix;
	} else {
		QString newPrefix = prefix;
		newPrefix.append(impl->m_prefix.left(1).toUpper());
		newPrefix.append(impl->m_prefix.mid(1));
		impl->m_prefix = newPrefix;
	}
}

QString XmlAttributeContainer::attName(const QString& name) const
{
	std::string prefix = iRIC::toStr(impl->m_prefix);

	if (impl->m_prefix.isEmpty()) {return name;}

	QString fullName = impl->m_prefix;
	fullName.append(name.left(1).toUpper());
	fullName.append(name.mid(1));

	std::string fn = iRIC::toStr(fullName);

	return fullName;
}

void XmlAttributeContainer::copyValue(const XmlAttributeContainer&)
{}
