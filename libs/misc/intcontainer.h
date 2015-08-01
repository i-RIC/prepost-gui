#ifndef INTCONTAINER_H
#define INTCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

class MISCDLL_EXPORT IntContainer : public SimpleValueContainerT<int>
{

public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	IntContainer(const QString& name);
	/// Constructor (with default value)
	IntContainer(const QString& name, int defaultVal);
	/// Constructor (copy)
	IntContainer(const IntContainer& c);
	/// Destructor
	virtual ~IntContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	IntContainer& operator=(int val);

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // INTCONTAINER_H
