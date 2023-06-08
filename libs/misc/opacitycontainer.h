#ifndef OPACITYCONTAINER_H
#define OPACITYCONTAINER_H

#include "misc_global.h"
#include "intcontainer.h"

class MISCDLL_EXPORT OpacityContainer : public IntContainer
{
public:
	OpacityContainer();
	OpacityContainer(const QString& name);
	OpacityContainer(const QString& name, int defaultVal);
	~OpacityContainer();

	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;
	OpacityContainer& operator=(const OpacityContainer& c);
	OpacityContainer& operator=(int val);
	/// Operator to convert to double
	operator double() const;
};

#endif // OPACITYCONTAINER_H
