#ifndef ENUMCONTAINERT_H
#define ENUMCONTAINERT_H

#include "intcontainer.h"

template <typename V>
class EnumContainerT : public IntContainer
{

public:
	/// @name Constructors and Destructor
	//@{
	EnumContainerT(const QString& name);
	EnumContainerT(const QString& name, V defaultVal);
	EnumContainerT(const EnumContainerT<V>& c);
	virtual ~EnumContainerT();

	/// @name Operators
	//@{
	EnumContainerT<V>& operator=(V val);
	bool operator==(V val) const;
	bool operator!=(V val) const;
	operator V() const;
	//@}

	/// @name Getter and Setter
	//@{
	V value() const;
	void setValue(const V& val);
	//@}
};

#include "private/enumcontainert_detail.h"

#endif // ENUMCONTAINERT_H
