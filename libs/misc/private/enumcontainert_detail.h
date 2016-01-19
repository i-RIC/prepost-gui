#ifndef ENUMCONTAINERT_DETAIL_H
#define ENUMCONTAINERT_DETAIL_H

#include "../enumcontainert.h"

template <typename V>
EnumContainerT<V>::EnumContainerT(const QString& name) :
	IntContainer {name}
{}

template <typename V>
EnumContainerT<V>::EnumContainerT(const QString& name, V defaultVal) :
	IntContainer {name, static_cast<int> (defaultVal)}
{}

template <typename V>
EnumContainerT<V>::EnumContainerT(const EnumContainerT<V>& c) :
	IntContainer {c}
{}

template <typename V>
EnumContainerT<V>::~EnumContainerT()
{}

template <typename V>
EnumContainerT<V>& EnumContainerT<V>::operator=(V val)
{
	IntContainer::operator=(static_cast<int>(val));
	return *this;
}

template <typename V>
bool EnumContainerT<V>::operator==(V val) const
{
	return IntContainer::operator ==(static_cast<int> (val));
}

template <typename V>
bool EnumContainerT<V>::operator!=(V val) const
{
	return IntContainer::operator !=(static_cast<int> (val));
}

template <typename V>
EnumContainerT<V>::operator V() const
{
	return static_cast<V>(IntContainer::operator int());
}

template <typename V>
V EnumContainerT<V>::value() const
{
	return static_cast<V> (IntContainer::value());
}

template <typename V>
void EnumContainerT<V>::setValue(const V& val)
{
	IntContainer::setValue(static_cast<int> (val));
}

#endif // ENUMCONTAINERT_DETAIL_H
