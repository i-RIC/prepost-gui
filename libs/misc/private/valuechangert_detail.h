#ifndef VALUECHANGERT_DETAIL_H
#define VALUECHANGERT_DETAIL_H

#include "../valuechangert.h"

template <typename T>
ValueChangerT<T>::ValueChangerT(T* pointer, T value) :
	m_pointer {pointer},
	m_restorer (pointer, *pointer)
{
	*m_pointer = value;
}

template <typename T>
ValueChangerT<T>::~ValueChangerT()
{}

template <typename T>
void ValueChangerT<T>::restore()
{
	m_restorer.restore();
}

#endif // VALUECHANGERT_DETAIL_H
