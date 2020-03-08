#include "qfontcontainer.h"
#include "xmlsupport.h"

#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamWriter>

QFontContainer::QFontContainer(const QString& name) :
	SimpleValueContainerT<QFont> {name}
{}

QFontContainer::QFontContainer(const QString& name, const QFont& defaultVal) :
	SimpleValueContainerT<QFont> {name, defaultVal}
{}

QFontContainer::QFontContainer(const QFontContainer& c) :
	SimpleValueContainerT<QFont> {c}
{}

QFontContainer::~QFontContainer()
{}

void QFontContainer::load(const QDomNode& node)
{
	auto elem = node.toElement();
	auto strVal = elem.attribute(attName(m_name));
	if (strVal.isNull()) {
		m_value = m_defaultValue;
	} else {
		m_value.fromString(strVal);
	}
}

void QFontContainer::save(QXmlStreamWriter& writer) const
{
	writer.writeAttribute(attName(m_name), m_value.toString());
}

QFontContainer& QFontContainer::operator=(const QFont& val)
{
	SimpleValueContainerT<QFont>::operator =(val);
	return *this;
}

bool QFontContainer::operator!=(const QFont& val)
{
	return SimpleValueContainerT<QFont>::operator !=(val);
}

void QFontContainer::copyValue(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const QFontContainer&> (c);
	m_value = c2.m_value;
}
