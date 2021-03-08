#include "colorcontainer.h"
#include "simplevaluecontainert.h"
#include "xmlsupport.h"
#include "private/colorcontainer_impl.h"

ColorContainer::Impl::Impl(const QString& name) :
	SimpleValueContainerT<QColor> (name, Qt::black)
{}

ColorContainer::Impl::Impl(const QString& name, const QColor& defaultVal) :
	SimpleValueContainerT<QColor> (name, defaultVal)
{}

ColorContainer::Impl::Impl(const Impl& i) :
	SimpleValueContainerT<QColor> (i)
{}

ColorContainer::Impl::~Impl()
{}

void ColorContainer::Impl::load(const QDomNode& node)
{
	m_value = iRIC::getColorAttribute(node, attName(m_name), m_defaultValue);
}

void ColorContainer::Impl::save(QXmlStreamWriter& writer) const
{
	iRIC::setColorAttribute(writer, attName(m_name), m_value);
}

// public interfaces

ColorContainer::ColorContainer(const QString& name) :
	XmlAttributeContainer {},
	impl {new Impl {name}}
{}

ColorContainer::ColorContainer(const QString& name, const QColor& defaultVal) :
	XmlAttributeContainer {},
	impl {new Impl {name, defaultVal}}
{}

ColorContainer::ColorContainer(const ColorContainer& c) :
	XmlAttributeContainer (c),
	impl {new Impl {*(c.impl)}}
{}

ColorContainer::~ColorContainer()
{
	delete impl;
	delete[] m_double;
}

void ColorContainer::load(const QDomNode& node)
{
	impl->load(node);
	updateVtkValue();
}

void ColorContainer::save(QXmlStreamWriter& writer) const
{
	impl->save(writer);
}

ColorContainer& ColorContainer::operator=(const ColorContainer& c)
{
	*(impl) = *(c.impl);
	updateVtkValue();
	return *this;
}

ColorContainer& ColorContainer::operator=(const QColor& v)
{
	impl->setValue(v);
	updateVtkValue();
	return *this;
}

ColorContainer::operator QColor() const
{
	return QColor(*impl);
}

ColorContainer::operator double*() const
{
	setupVtkValue();
	return m_double;
}

const QColor& ColorContainer::value() const
{
	return impl->value();
}

void ColorContainer::setValue(const QColor& val)
{
	impl->setValue(val);
}

const QString& ColorContainer::prefix() const
{
	return impl->prefix();
}

void ColorContainer::addPrefix(const QString& prefix)
{
	impl->addPrefix(prefix);
}

QString ColorContainer::attName(const QString& name) const
{
	return impl->attName(name);
}

void ColorContainer::copyValue(const XmlAttributeContainer& c)
{
	const ColorContainer& c2 = dynamic_cast<const ColorContainer&> (c);
	impl->setValue(c2.impl->value());
}

void ColorContainer::setupVtkValue() const
{
	if (m_double == nullptr) {
		m_double = new double[3];
	}
	updateVtkValue();
}

void ColorContainer::updateVtkValue() const
{
	if (m_double == nullptr) {return;}
	QColor c = impl->value();
	*(m_double) = c.redF();
	*(m_double + 1) = c.greenF();
	*(m_double + 2) = c.blueF();
}
