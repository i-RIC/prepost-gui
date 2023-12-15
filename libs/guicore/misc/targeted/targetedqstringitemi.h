#ifndef TARGETEDQSTRINGITEMI_H
#define TARGETEDQSTRINGITEMI_H

#include <QString>

class TargetedQStringItemI
{
public:
	virtual ~TargetedQStringItemI() {}

	virtual QString target() const = 0;
	virtual void setTarget(const QString& target) = 0;
};

#endif // TARGETEDQSTRINGITEMI_H
