#ifndef TARGETEDITEMI_H
#define TARGETEDITEMI_H

#include <string>

class TargetedItemI
{
public:
	virtual ~TargetedItemI() {}

	virtual std::string target() const = 0;
	virtual void setTarget(const std::string& target) = 0;
};

#endif // TARGETEDITEMI_H
