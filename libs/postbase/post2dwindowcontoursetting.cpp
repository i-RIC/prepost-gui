#include "post2dwindowcontoursetting.h"

const int Post2dWindowContourSetting::DEFAULT_NUMOFDIV = 15;

Post2dWindowContourSetting::Post2dWindowContourSetting() :
	CompositeContainer
		({&numberOfDivisions, &target, &contour, &fillUpper, &fillLower,
		 &opacity, &regionMode, &range, &scalarBarSetting, &titleTextSetting, &labelTextSetting}),
	numberOfDivisions {"numberOfDivisions", DEFAULT_NUMOFDIV},
	target {"solution"},
	contour {"contour", ContourSettingWidget::ColorFringe},
	fillUpper {"fillUpper", true},
	fillLower {"fillLower", true},
	regionMode {"regionMode", StructuredGridRegion::rmFull}
{
	opacity = 50;
	range.setPrefix("region");
	titleTextSetting.setPrefix("titleText");
	labelTextSetting.setPrefix("labelText");
}

Post2dWindowContourSetting::Post2dWindowContourSetting(const Post2dWindowContourSetting& s) :
	Post2dWindowContourSetting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowContourSetting::~Post2dWindowContourSetting()
{}

Post2dWindowContourSetting& Post2dWindowContourSetting::operator=(const Post2dWindowContourSetting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}
