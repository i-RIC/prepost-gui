#ifndef VALUEARRAYCONTAINERT_H
#define VALUEARRAYCONTAINERT_H

#include "xmlattributecontainer.h"

#include <vector>

template <typename V>
class ValueArrayContainerT : public XmlAttributeContainer
{
public:
	/// @name Constructors and Destructor
	//@{
	ValueArrayContainerT(const QString& name);
	ValueArrayContainerT(const QString& name, const std::vector<V>& vals);
	ValueArrayContainerT(const ValueArrayContainerT<V>& c);
	virtual ~ValueArrayContainerT();
	//@}

	/// @name Operators
	//@{
	ValueArrayContainerT<V>& operator=(const ValueArrayContainerT<V>& c);
	ValueArrayContainerT<V>& operator=(const std::vector<V>& v);
	bool operator==(const std::vector<V>& v) const;
	bool operator!=(const std::vector<V>& v) const;
	//@}

	/// @name Getter and Setter
	//@{
	const std::vector<V>& value() const;
	void setValue(const std::vector<V>& val);
	//@}

protected:
	QString m_name;
	std::vector<V> m_value;
};

#include "private/valuearraycontainert_detail.h"

#endif // VALUEARRAYCONTAINERT_H
