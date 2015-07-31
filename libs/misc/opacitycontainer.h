#ifndef OPACITYCONTAINER_H
#define OPACITYCONTAINER_H

#include "misc_global.h"
#include "intcontainer.h"

class MISCDLL_EXPORT OpacityContainer : public IntContainer
{
public:
	/// Constructor
	OpacityContainer();
	/// Constructor with name
	OpacityContainer(const QString& name);
	/// Destructor
	~OpacityContainer();

	/// Operator to copy
	OpacityContainer& operator=(const OpacityContainer& c);
	/// Operator to copy
	OpacityContainer& operator=(int val);
	/// Operator to convert to double
	operator double() const;
};

#endif // OPACITYCONTAINER_H
