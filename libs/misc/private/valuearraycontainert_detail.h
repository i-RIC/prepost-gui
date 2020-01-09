#ifndef VALUEARRAYCONTAINERT_DETAIL_H
#define VALUEARRAYCONTAINERT_DETAIL_H

#include "../valuearraycontainert.h"

template <typename V>
ValueArrayContainerT<V>::ValueArrayContainerT(const QString& name) :
	XmlAttributeContainer {},
	m_name {name},
	m_value {}
{}

template <typename V>
ValueArrayContainerT<V>::ValueArrayContainerT(const QString& name, const std::vector<V>& vals) :
	XmlAttributeContainer {},
	m_name {name},
	m_value (vals)
{}

template <typename V>
ValueArrayContainerT<V>::ValueArrayContainerT(const ValueArrayContainerT<V>& c) :
	XmlAttributeContainer {c},
	m_name {c.m_name},
	m_value (c.m_value)
{}

template <typename V>
ValueArrayContainerT<V>::~ValueArrayContainerT()
{}

template <typename V>
ValueArrayContainerT<V>& ValueArrayContainerT<V>::operator=(const ValueArrayContainerT<V>& c)
{
	XmlAttributeContainer::operator =(c);
	m_name = c.m_name;
	m_value = c.m_value;
	return *this;
}

template <typename V>
ValueArrayContainerT<V>& ValueArrayContainerT<V>::operator=(const std::vector<V>& v)
{
	m_value = v;
	return *this;
}

template <typename V>
bool ValueArrayContainerT<V>::operator==(const std::vector<V>& v) const
{
	return m_value == v;
}

template <typename V>
bool ValueArrayContainerT<V>::operator!=(const std::vector<V>& v) const
{
	return m_value != v;
}

template <typename V>
const std::vector<V>& ValueArrayContainerT<V>::value() const
{
	return m_value;
}

template <typename V>
void ValueArrayContainerT<V>::setValue(const std::vector<V>& val)
{
	m_value = val;
}

#endif // VALUEARRAYCONTAINERT_DETAIL_H
