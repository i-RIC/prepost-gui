#ifndef NUMERICVALUECONTAINERT_DETAIL_H
#define NUMERICVALUECONTAINERT_DETAIL_H

#include "../numericvaluecontainert.h"

template <typename V>
NumericValueContainerT<V>::NumericValueContainerT(const QString& name) :
	SimpleValueContainerT<V> (name)
{}

template <typename V>
NumericValueContainerT<V>::NumericValueContainerT(const QString& name, const V& defaultVal) :
	SimpleValueContainerT<V> (name, defaultVal)
{}

template <typename V>
NumericValueContainerT<V>::NumericValueContainerT(const NumericValueContainerT<V>& c) :
	SimpleValueContainerT<V> (c)
{}

template <typename V>
NumericValueContainerT<V>::~NumericValueContainerT()
{}

template <typename V>
NumericValueContainerT<V>& NumericValueContainerT<V>::operator+=(const V& v)
{
	m_value += v;
	return *this;
}

template <typename V>
NumericValueContainerT<V>& NumericValueContainerT<V>::operator-=(const V& v)
{
	m_value -= v;
	return *this;
}

#endif // NUMERICVALUECONTAINERT_DETAIL_H
