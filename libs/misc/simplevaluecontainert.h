#ifndef SIMPLEVALUECONTAINERT_H
#define SIMPLEVALUECONTAINERT_H

#include "xmlattributecontainer.h"

#include <QString>

template <typename V>
class SimpleValueContainerT : public XmlAttributeContainer
{
public:
	/// @name Constructors and Destructor
	//@{
	SimpleValueContainerT(const QString& name);
	SimpleValueContainerT(const QString& name, const V& defaultVal);
	SimpleValueContainerT(const SimpleValueContainerT<V>& c);
	virtual ~SimpleValueContainerT();
	//@}

	/// @name Operators
	//@{
	SimpleValueContainerT<V>& operator=(const SimpleValueContainerT<V>& c);
	SimpleValueContainerT<V>& operator=(const V& v);
	bool operator==(const V& v) const;
	bool operator!=(const V& v) const;
	operator V() const;
	//@}

	/// @name Getter and Setter
	//@{
	const V& value() const;
	void setValue(const V& val);
	//@}

protected:
	QString m_name;
	V m_value;
	V m_defaultValue;
};

#include "private/simplevaluecontainert_detail.h"

#endif // SIMPLEVALUECONTAINERT_H
