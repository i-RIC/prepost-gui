#ifndef INTARRAYCONTAINER_H
#define INTARRAYCONTAINER_H

#include "misc_global.h"
#include "valuearraycontainert.h"

#include <QString>

class MISCDLL_EXPORT IntArrayContainer : public ValueArrayContainerT<int>
{
public:
	/// @name Constructors and Destructor
	//@{
	IntArrayContainer(const QString& name);
	IntArrayContainer(const QString& name, const std::vector<int>& vals);
	IntArrayContainer(const IntArrayContainer& c);
	~IntArrayContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	IntArrayContainer& operator=(const std::vector<int>& val);

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // INTARRAYCONTAINER_H
