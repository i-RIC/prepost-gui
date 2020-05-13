#ifndef VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGET_H
#define VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGET_H

#include <QDockWidget>

class VerticalCrossSectionWindowElevationsDockWidgetView;

class VerticalCrossSectionWindowElevationsDockWidget : public QDockWidget
{
public:
	VerticalCrossSectionWindowElevationsDockWidget(QWidget* parent);

	VerticalCrossSectionWindowElevationsDockWidgetView* view() const;
};

#endif // VERTICALCROSSSECTIONWINDOWELEVATIONSDOCKWIDGET_H
