#ifndef POSTTITLESETTING_H
#define POSTTITLESETTING_H

#include "../postbase_global.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/stringcontainer.h>

class POSTBASEDLL_EXPORT PostTitleSetting : public CompositeContainer
{
public:
	PostTitleSetting();
	PostTitleSetting(const PostTitleSetting& setting);

	PostTitleSetting& operator=(const PostTitleSetting& setting);

	StringContainer title;
	IntContainer fontSize;
	ColorContainer color;
};

#endif // POSTTITLESETTING_H
