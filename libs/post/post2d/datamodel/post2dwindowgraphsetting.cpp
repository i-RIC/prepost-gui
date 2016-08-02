#include "post2dwindowgraphsetting.h"

Post2dWindowGraphSetting::Post2dWindowGraphSetting() :
	CompositeContainer({&direction, &region,
		&regionStartIndex, &regionEndIndex, &regionSkipIndex, &regionIndices,
		&offsetDirection, &drawGridLine, &gridLineStyle,
		&graphTarget, &graphScale, &graphLineStyle, &graphValueFix}),

	direction {"direction", dirI},
	region {"region", All},
	regionStartIndex {"regionStart"},
	regionEndIndex {"regionEnd"},
	regionSkipIndex {"regionSkip", 1},
	regionIndices {"regionIndices"},
	offsetDirection {"offsetDirection", Vertical},
	drawGridLine {"drawGridLine", true},
	gridLineStyle {},
	graphTarget {"solution"},
	graphScale {"scale", 1},
	graphLineStyle {},
	graphValueFix {"valueFix", NoFix}
{
	gridLineStyle.setPrefix("grid");
	gridLineStyle.color = Qt::gray;

	graphLineStyle.setPrefix("graph");
	graphLineStyle.color = Qt::black;
}

Post2dWindowGraphSetting::Post2dWindowGraphSetting(const Post2dWindowGraphSetting& setting) :
	Post2dWindowGraphSetting()
{
	copyValue(setting);
}

Post2dWindowGraphSetting::~Post2dWindowGraphSetting()
{}

Post2dWindowGraphSetting& Post2dWindowGraphSetting::operator=(const Post2dWindowGraphSetting& c)
{
	CompositeContainer::copyValue(c);
	return *this;
}

XmlAttributeContainer& Post2dWindowGraphSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post2dWindowGraphSetting&>(c));
}
