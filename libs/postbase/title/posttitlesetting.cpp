#include "posttitlesetting.h"

#include <QColor>

PostTitleSetting::PostTitleSetting() :
	CompositeContainer({&title, &fontSetting}),
	title {"title"},
	fontSetting {}
{
	fontSetting.fontSize = 20;
}

PostTitleSetting::PostTitleSetting(const PostTitleSetting &setting) :
	PostTitleSetting()
{
	copyValue(setting);
}

PostTitleSetting& PostTitleSetting::operator=(const PostTitleSetting &setting)
{
	copyValue(setting);
	return *this;
}
