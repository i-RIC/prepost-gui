#ifndef DOUBLECONTAINER_H
#define DOUBLECONTAINER_H

#include "misc_global.h"
#include "numericvaluecontainert.h"

class MISCDLL_EXPORT DoubleContainer : public NumericValueContainerT<double>
{
public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	DoubleContainer(const QString& name);
	/// Constructor (with default value)
	DoubleContainer(const QString& name, double defaultVal);
	/// Constructor (copy)
	DoubleContainer(const DoubleContainer& c);
	/// Destructor
	virtual ~DoubleContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	DoubleContainer& operator=(double val);

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // DOUBLECONTAINER_H
