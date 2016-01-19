#ifndef BOOLCONTAINER_H
#define BOOLCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

class MISCDLL_EXPORT BoolContainer : public SimpleValueContainerT<bool>
{

public:
	/// @name Constructors and Destructor
	//@{
	BoolContainer(const QString& name);
	BoolContainer(const QString& name, bool defaultVal);
	BoolContainer(const BoolContainer& c);
	virtual ~BoolContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	//@{
	BoolContainer& operator=(bool val);
	//@}

private:
	void copyValue(const XmlAttributeContainer& c) override;
};

#endif // BOOLCONTAINER_H
