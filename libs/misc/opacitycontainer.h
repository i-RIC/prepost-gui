#ifndef OPACITYCONTAINER_H
#define OPACITYCONTAINER_H

#include "misc_global.h"

#include "boolcontainer.h"
#include "compositecontainer.h"
#include "intcontainer.h"

class MISCDLL_EXPORT OpacityContainer : public CompositeContainer
{
public:
	OpacityContainer();
	OpacityContainer(const OpacityContainer& c);
	OpacityContainer(const QString& name);
	OpacityContainer(const QString& name, int defaultVal);
	~OpacityContainer();

	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;
	OpacityContainer& operator=(const OpacityContainer& c);
	OpacityContainer& operator=(int val);

	bool operator==(const OpacityContainer& c) const;

	int opacityPercent() const;
	int value() const;
	void setValue(int value);

	/// Operator to convert to double
	operator double() const;

	BoolContainer enabled;
	IntContainer percent;
};

#endif // OPACITYCONTAINER_H
