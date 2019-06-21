#ifndef POSTTITLESETTING_H
#define POSTTITLESETTING_H

#include "../postbase_global.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class POSTBASEDLL_EXPORT PostTitleSetting : public CompositeContainer
{
public:
	PostTitleSetting();
	PostTitleSetting(const PostTitleSetting& setting);

	PostTitleSetting& operator=(const PostTitleSetting& setting);

	StringContainer title;
	vtkTextPropertySettingContainer fontSetting;
};

#endif // POSTTITLESETTING_H
