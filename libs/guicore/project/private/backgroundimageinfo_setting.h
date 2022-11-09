#ifndef BACKGROUNDIMAGEINFO_SETTING_H
#define BACKGROUNDIMAGEINFO_SETTING_H

#include "../backgroundimageinfo.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>
#include <misc/stringcontainer.h>

class BackgroundImageInfo::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);
	~Setting();

	Setting& operator=(const Setting& setting);

	StringContainer fileName;
	DoubleContainer positionX;
	DoubleContainer positionY;
	DoubleContainer scale;
	DoubleContainer angle;

	BoolContainer fixed;
};

#endif // BACKGROUNDIMAGEINFO_SETTING_H
