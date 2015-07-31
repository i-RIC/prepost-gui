#ifndef ENUMCONTAINERT_H
#define ENUMCONTAINERT_H

#include "intcontainer.h"

template <typename V>
class EnumContainerT : public IntContainer
{
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	EnumContainerT(const QString& name) :
		IntContainer {name}
	{}
	/// Constructor (with default value)
	EnumContainerT(const QString& name, V defaultVal) :
		IntContainer {name, static_cast<int> (defaultVal)}
	{}
	/// Constructor (copy)
	EnumContainerT(const EnumContainerT<V>& c) :
		IntContainer {c}
	{}
	/// Destructor
	~EnumContainerT() {}

	/// @name Operators
	//@{
	EnumContainerT<V>& operator=(T val)
	{
		return operator=(static_cast<int>(val));
	}
	operator T() const
	{
		return static_cast<T>(int());
	}
	//@}
};


#endif // ENUMCONTAINERT_H

