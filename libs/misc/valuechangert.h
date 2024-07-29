#ifndef VALUECHANGERT_H
#define VALUECHANGERT_H

#include "valuerestorert.h"

template <typename T>
class ValueChangerT
{
public:
	ValueChangerT(T* pointer, T value);
	~ValueChangerT();

	void restore();

private:
	T* m_pointer;
	ValueRestorerT<T> m_restorer;
};

#include "private/valuechangert_detail.h"

#endif // VALUECHANGERT_H
