#include "verticalcrosssectionwindowdisplaysettingdockwidget.h"
#include "verticalcrosssectionwindowdisplaysettingdockwidgetview.h"

VerticalCrossSectionWindowDisplaySettingDockWidget::VerticalCrossSectionWindowDisplaySettingDockWidget(QWidget* parent) :
	QDockWidget(tr("Visibility Setting"), parent)
{
	setFeatures(QDockWidget::DockWidgetMovable);

	auto view = new VerticalCrossSectionWindowDisplaySettingDockWidgetView(this);
	setWidget(view);
}

VerticalCrossSectionWindowDisplaySettingDockWidgetView* VerticalCrossSectionWindowDisplaySettingDockWidget::view() const
{
	return dynamic_cast<VerticalCrossSectionWindowDisplaySettingDockWidgetView*> (widget());
}
