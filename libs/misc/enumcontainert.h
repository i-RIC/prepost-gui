#ifndef ENUMCONTAINERT_H
#define ENUMCONTAINERT_H

#include "intcontainer.h"

template <typename V>
class EnumContainerT : public IntContainer
{

public:
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
	EnumContainerT<V>& operator=(V val)
	{
		IntContainer::operator=(static_cast<int>(val));
		return *this;
	}
	bool operator==(V val) {
		return IntContainer::operator ==(static_cast<int> (val));
	}
	bool operator!=(V val) {
		return IntContainer::operator !=(static_cast<int> (val));
	}
	operator V() const
	{
		return static_cast<V>(IntContainer::operator int());
	}
	//@}

	/// @name Getter and Setter
	/// Value getter
	V value() const
	{
		return static_cast<V> (IntContainer::value());
	}
	/// Value setter
	void setValue(const V& val)
	{
		IntContainer::setValue(static_cast<int> (val));
	}
};


#endif // ENUMCONTAINERT_H

