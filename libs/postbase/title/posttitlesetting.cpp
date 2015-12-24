#include "posttitlesetting.h"

#include <QColor>

PostTitleSetting::PostTitleSetting() :
	CompositeContainer({&title, &fontSize, &color}),
	title {"title"},
	fontSize {"fontSize", 20},
	color {"color", Qt::black}
{}

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
