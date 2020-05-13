#include "verticalcrosssectionwindowelevationsdockwidget.h"
#include "verticalcrosssectionwindowelevationsdockwidgetview.h"

VerticalCrossSectionWindowElevationsDockWidget::VerticalCrossSectionWindowElevationsDockWidget(QWidget* parent) :
	QDockWidget(tr("WSE Edit Tool"), parent)
{
	setFeatures(QDockWidget::DockWidgetMovable);

	auto view = new VerticalCrossSectionWindowElevationsDockWidgetView(this);
	setWidget(view);
}

VerticalCrossSectionWindowElevationsDockWidgetView* VerticalCrossSectionWindowElevationsDockWidget::view() const
{
	return dynamic_cast<VerticalCrossSectionWindowElevationsDockWidgetView*> (widget());
}
