#ifndef INTCONTAINER_H
#define INTCONTAINER_H

#include "misc_global.h"
#include "numericvaluecontainert.h"

class MISCDLL_EXPORT IntContainer : public NumericValueContainerT<int>
{
public:
	/// @name Constructors and Destructor
	//@{
	IntContainer(const QString& name);
	IntContainer(const QString& name, int defaultVal);
	IntContainer(const IntContainer& c);
	virtual ~IntContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	IntContainer& operator=(int val);

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // INTCONTAINER_H
