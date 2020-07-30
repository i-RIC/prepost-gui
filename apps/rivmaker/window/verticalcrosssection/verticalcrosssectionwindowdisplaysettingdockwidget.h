#ifndef VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDOCKWIDGET_H
#define VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDOCKWIDGET_H

#include <QDockWidget>

class VerticalCrossSectionWindowDisplaySettingDockWidgetView;

class VerticalCrossSectionWindowDisplaySettingDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	VerticalCrossSectionWindowDisplaySettingDockWidget(QWidget* parent);

	VerticalCrossSectionWindowDisplaySettingDockWidgetView* view() const;
};

#endif // VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDOCKWIDGET_H
