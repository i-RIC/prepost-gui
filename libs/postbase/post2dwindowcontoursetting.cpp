#include "post2dwindowcontoursetting.h"

const int Post2dWindowContourSetting::DEFAULT_NUMOFDIV = 15;

Post2dWindowContourSetting::Post2dWindowContourSetting() :
	CompositeContainer
		({&numberOfDivisions, &currentSolution, &contour, &fillUpper, &fillLower,
		 &opacity, &regionMode, &range, &scalarBarSetting, &titleTextSetting, &labelTextSetting}),
	numberOfDivisions {"numberOfDivisions", DEFAULT_NUMOFDIV},
	currentSolution {"solution"},
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
