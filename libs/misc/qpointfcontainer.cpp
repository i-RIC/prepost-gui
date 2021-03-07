#include "qpointfcontainer.h"
#include "private/qpointfcontainer_impl.h"

QPointFContainer::QPointFContainer() :
	XmlAttributeContainer {},
	impl {new Impl {}}
{}

QPointFContainer::QPointFContainer(const QString& name) :
	QPointFContainer {}
{
	addPrefix(name);
}

QPointFContainer::QPointFContainer(const QPointF& val) :
	QPointFContainer {}
{
	impl->m_xValue = val.x();
	impl->m_yValue = val.y();
}

QPointFContainer::QPointFContainer(const QPointFContainer& c) :
	QPointFContainer {}
{
	impl->m_xValue = c.impl->m_xValue.value();
	impl->m_yValue = c.impl->m_yValue.value();
}

QPointFContainer::~QPointFContainer()
{}

QPointFContainer& QPointFContainer::operator=(const QPointFContainer& c)
{
	impl->m_xValue = c.impl->m_xValue.value();
	impl->m_yValue = c.impl->m_yValue.value();
	return *this;
}

QPointFContainer& QPointFContainer::operator=(const QPointF& v)
{
	impl->m_xValue = v.x();
	impl->m_yValue = v.y();
	return *this;
}

QPointFContainer& QPointFContainer::operator+=(const QPointF& v)
{
	impl->m_xValue += v.x();
	impl->m_yValue += v.y();
	return *this;
}

QPointFContainer& QPointFContainer::operator-=(const QPointF& v)
{
	impl->m_xValue -= v.x();
	impl->m_yValue -= v.y();
	return *this;
}

bool QPointFContainer::operator==(const QPointF& v) const
{
	return (impl->m_xValue == v.x() && impl->m_yValue == v.y());
}
bool QPointFContainer::operator!=(const QPointF& v) const
{
	return (impl->m_xValue != v.x() || impl->m_yValue != v.y());
}
QPointFContainer::operator QPointF() const
{
	return QPointF(impl->m_xValue, impl->m_yValue);
}

void QPointFContainer::load(const QDomNode& node)
{
	impl->m_xValue.load(node);
	impl->m_yValue.load(node);
}

void QPointFContainer::save(QXmlStreamWriter& writer) const
{
	impl->m_xValue.save(writer);
	impl->m_yValue.save(writer);
}

QPointF QPointFContainer::value() const
{
	return QPointF(impl->m_xValue, impl->m_yValue);
}

void QPointFContainer::setValue(const QPointF& val)
{
	impl->m_xValue = val.x();
	impl->m_yValue = val.y();
}

void QPointFContainer::addPrefix(const QString& prefix)
{
	impl->m_xValue.addPrefix(prefix);
	impl->m_yValue.addPrefix(prefix);
	XmlAttributeContainer::addPrefix(prefix);
}

void QPointFContainer::copyValue(const XmlAttributeContainer& c)
{
	const QPointFContainer& c2 = dynamic_cast<const QPointFContainer&> (c);
	impl->m_xValue = c2.impl->m_xValue;
	impl->m_yValue = c2.impl->m_yValue;
}
