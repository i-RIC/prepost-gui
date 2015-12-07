#ifndef SIMPLEVALUECONTAINERT_DETAIL_H
#define SIMPLEVALUECONTAINERT_DETAIL_H

#include "../simplevaluecontainert.h"

template <typename V>
SimpleValueContainerT<V>::SimpleValueContainerT(const QString& name) :
	XmlAttributeContainer {},
	m_name {name},
	m_value {},
	m_defaultValue {}
{}

template <typename V>
SimpleValueContainerT<V>::SimpleValueContainerT(const QString& name, const V& defaultVal) :
	XmlAttributeContainer {},
	m_name {name},
	m_value {defaultVal},
	m_defaultValue {defaultVal}
{}

template <typename V>
SimpleValueContainerT<V>::SimpleValueContainerT(const SimpleValueContainerT<V>& c) :
	XmlAttributeContainer {c},
	m_name {c.m_name},
	m_value {c.m_value},
	m_defaultValue {c.m_defaultValue}
{}

template <typename V>
SimpleValueContainerT<V>::~SimpleValueContainerT()
{}

template <typename V>
SimpleValueContainerT<V>& SimpleValueContainerT<V>::operator=(const SimpleValueContainerT<V>& c)
{
	XmlAttributeContainer::operator =(c);
	m_name = c.m_name;
	m_value = c.m_value;
	m_defaultValue = c.m_defaultValue;
	return *this;
}

template <typename V>
SimpleValueContainerT<V>& SimpleValueContainerT<V>::operator=(const V& v)
{
	m_value = v;
	return *this;
}

template <typename V>
bool SimpleValueContainerT<V>::operator==(const V& v) const
{
	return m_value == v;
}

template <typename V>
bool SimpleValueContainerT<V>::operator!=(const V& v) const
{
	return m_value != v;
}

template <typename V>
SimpleValueContainerT<V>::operator V() const
{
	return m_value;
}

template <typename V>
const V& SimpleValueContainerT<V>::value() const
{
	return m_value;
}

template <typename V>
void SimpleValueContainerT<V>::setValue(const V& val)
{
	m_value = val;
}

#endif // SIMPLEVALUECONTAINERT_DETAIL_H
