#ifndef VALUERESTORERT_H
#define VALUERESTORERT_H

template <typename T>
class ValueRestorerT
{
public:
	ValueRestorerT(T* pointer, T value);
	~ValueRestorerT();

private:
	T* m_pointer;
	T m_value;
};

#include "private/valuerestorert_detail.h"

#endif // VALUERESTORERT_H
