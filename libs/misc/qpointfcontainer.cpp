#include "qpointfcontainer.h"

QPointFContainer::QPointFContainer() :
	XmlAttributeContainer {}
{}

QPointFContainer::QPointFContainer(const QString& name) :
	QPointFContainer {}
{
	setPrefix(name);
}

QPointFContainer::QPointFContainer(const QPointF& val) :
	QPointFContainer {}
{
	m_xValue = val.x();
	m_yValue = val.y();
}

QPointFContainer::QPointFContainer(const QPointFContainer& c) :
	QPointFContainer {}
{
	m_xValue = c.m_xValue.value();
	m_yValue = c.m_yValue.value();
}

QPointFContainer::~QPointFContainer()
{}

QPointFContainer& QPointFContainer::operator=(const QPointFContainer& c)
{
	m_xValue = c.m_xValue.value();
	m_yValue = c.m_yValue.value();
	return *this;
}

QPointFContainer& QPointFContainer::operator=(const QPointF& v)
{
	m_xValue = v.x();
	m_yValue = v.y();
	return *this;
}

QPointFContainer& QPointFContainer::operator+=(const QPointF& v)
{
	m_xValue += v.x();
	m_yValue += v.y();
	return *this;
}

QPointFContainer& QPointFContainer::operator-=(const QPointF& v)
{
	m_xValue -= v.x();
	m_yValue -= v.y();
	return *this;
}

bool QPointFContainer::operator==(const QPointF& v) const
{
	return (m_xValue == v.x() && m_yValue == v.y());
}
bool QPointFContainer::operator!=(const QPointF& v) const
{
	return (m_xValue != v.x() || m_yValue != v.y());
}
QPointFContainer::operator QPointF() const
{
	return QPointF(m_xValue, m_yValue);
}

void QPointFContainer::load(const QDomNode& node)
{
	m_xValue.load(node);
	m_yValue.load(node);
}

void QPointFContainer::save(QXmlStreamWriter& writer) const
{
	m_xValue.save(writer);
	m_yValue.save(writer);
}

QPointF QPointFContainer::value() const
{
	return QPointF(m_xValue, m_yValue);
}

void QPointFContainer::setValue(const QPointF& val)
{
	m_xValue = val.x();
	m_yValue = val.y();
}

void QPointFContainer::setPrefix(const QString& prefix)
{
	m_xValue.setPrefix(prefix);
	m_yValue.setPrefix(prefix);
	XmlAttributeContainer::setPrefix(prefix);
}
