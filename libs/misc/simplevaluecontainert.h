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
	/// Constructor
	SimpleValueContainerT(const QString& name) :
		XmlAttributeContainer {},
		m_name {name},
		m_value {},
		m_defaultValue {}
	{}
	/// Constructor (with default value)
	SimpleValueContainerT(const QString& name, const V& defaultVal) :
		XmlAttributeContainer {},
		m_name {name},
		m_value {defaultVal},
		m_defaultValue {defaultVal}
	{}
	/// Constructor (copy)
	SimpleValueContainerT(const SimpleValueContainerT<V>& c) :
		XmlAttributeContainer {c},
		m_name {c.m_name},
		m_value {c.m_value},
		m_defaultValue {c.m_defaultValue}
	{}
	/// Destructor
	virtual ~SimpleValueContainerT()
	{}
	//@}

	/// @name Operators
	//@{
	/// Copy operator
	SimpleValueContainerT<V>& operator=(const SimpleValueContainerT<V>& c)
	{
		XmlAttributeContainer::operator =(c);
		m_name = c.m_name;
		m_value = c.m_value;
		m_defaultValue = c.m_defaultValue;
		return *this;
	}
	SimpleValueContainerT<V>& operator=(const V& v)
	{
		m_value = v;
		return *this;
	}
	SimpleValueContainerT<V>& operator+=(const V& v)
	{
		m_value += v;
		return *this;
	}
	SimpleValueContainerT<V>& operator-=(const V& v)
	{
		m_value -= v;
		return *this;
	}
	bool operator==(const V& v) const
	{
		return m_value == v;
	}
	bool operator!=(const V& v) const
	{
		return m_value != v;
	}
	operator V() const {return m_value;}
	//@}

	/// @name Getter and Setter
	//@{
	/// Value getter
	const V& value() const {return m_value;}
	/// Value setter
	void setValue(const V& val) {m_value = val;}
	//@}

protected:
	QString m_name;
	V m_value;
	V m_defaultValue;
};

#endif // SIMPLEVALUECONTAINERT_H

