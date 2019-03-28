#include "postpolydatabasicsetting.h"

#include <QColor>
#include <QSettings>

PostPolyDataBasicSetting::PostPolyDataBasicSetting() :
	CompositeContainer ({&color, &lineWidth}),
	color {"color"},
	lineWidth {"lineWidth"}
{
	QSettings settings;
	color = settings.value("graphics/polydatacolor", QColor(Qt::black)).value<QColor>();
	lineWidth = 1;
}

PostPolyDataBasicSetting::PostPolyDataBasicSetting(const PostPolyDataBasicSetting& s) :
	PostPolyDataBasicSetting()
{
	CompositeContainer::copyValue(s);
}

PostPolyDataBasicSetting& PostPolyDataBasicSetting::operator=(const PostPolyDataBasicSetting& c)
{
	CompositeContainer::copyValue(c);
	return *this;
}
