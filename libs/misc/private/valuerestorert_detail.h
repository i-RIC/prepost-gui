#ifndef VALUERESTORERT_DETAIL_H
#define VALUERESTORERT_DETAIL_H

#include "../valuerestorert.h"

template <typename T>
ValueRestorerT<T>::ValueRestorerT(T* pointer, T value) :
	m_pointer {pointer},
	m_value {value}
{}

template <typename T>
ValueRestorerT<T>::~ValueRestorerT()
{
	*m_pointer = m_value;
}

#endif // VALUERESTORERT_DETAIL_H
