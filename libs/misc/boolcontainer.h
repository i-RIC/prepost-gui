#ifndef BOOLCONTAINER_H
#define BOOLCONTAINER_H

#include "misc_global.h"
#include "simplevaluecontainert.h"

class MISCDLL_EXPORT BoolContainer : public SimpleValueContainerT<bool>
{

public:
	/// @name Constructors and Destructor
	//@{
	/// Constructor
	BoolContainer(const QString& name);
	/// Constructor (with default value)
	BoolContainer(const QString& name, bool defaultVal);
	/// Constructor (copy)
	BoolContainer(const BoolContainer& c);
	/// Destructor
	virtual ~BoolContainer();
	//@}

	/// @name XML I/O functions
	//@{
	void load(const QDomNode&) override;
	/// Save data into XML node as attributes
	void save(QXmlStreamWriter&) const override;
	//@}

	/// @name Operators
	BoolContainer& operator=(bool val);
};

#endif // BOOLCONTAINER_H
