#include "scalarsettingcontainer.h"

const int ScalarSettingContainer::DEFAULT_NUMOFDIV = 15;

ScalarSettingContainer::ScalarSettingContainer() :
	CompositeContainer({&target, &contour, &numberOfDivisions,
			&fillUpper, &fillLower, &opacity, &scalarBarSetting}),
	target {"solution"},
	contour {"contour", ContourSettingWidget::ColorFringe},
	numberOfDivisions {"numberOfDivisions", DEFAULT_NUMOFDIV},
	fillUpper {"fillUpper", true},
	fillLower {"fillLower", true},
	opacity {},
	scalarBarSetting {}
{
	opacity = 50;
	scalarBarSetting.addPrefix("scalarBar");
}

ScalarSettingContainer::ScalarSettingContainer(const ScalarSettingContainer &c) :
	ScalarSettingContainer()
{
	copyValue(c);
}

ScalarSettingContainer::~ScalarSettingContainer()
{}


ScalarSettingContainer& ScalarSettingContainer::operator=(const ScalarSettingContainer& c)
{
	CompositeContainer::copyValue(c);
	return *this;
}

XmlAttributeContainer& ScalarSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ScalarSettingContainer&>(c));
}
