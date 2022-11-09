#ifndef NUMERICVALUECONTAINERT_H
#define NUMERICVALUECONTAINERT_H

#include "simplevaluecontainert.h"

template <typename V>
class NumericValueContainerT : public SimpleValueContainerT<V>
{
public:
	/// @name Constructors and Destructor
	//@{
	NumericValueContainerT(const QString& name);
	NumericValueContainerT(const QString& name, const V& defaultVal);
	NumericValueContainerT(const NumericValueContainerT<V>& c);
	virtual ~NumericValueContainerT();
	//@}

	/// @name Operators
	//@{
	NumericValueContainerT<V>& operator+=(const V& v);
	NumericValueContainerT<V>& operator-=(const V& v);
	NumericValueContainerT<V>& operator*=(const V& v);
	NumericValueContainerT<V>& operator/=(const V& v);
	//@}
};

#include "private/numericvaluecontainert_detail.h"

#endif // NUMERICVALUECONTAINERT_H
