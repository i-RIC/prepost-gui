#ifndef VERTICALCROSSSECTIONDISPLAYWINDOWSETTINGDOCKWIDGETVIEW_H
#define VERTICALCROSSSECTIONDISPLAYWINDOWSETTINGDOCKWIDGETVIEW_H

#include <QWidget>

namespace Ui {
class VerticalCrossSectionWindowDisplaySettingDockWidgetView;
}

class VerticalCrossSectionWindowDisplaySettingDockWidgetView : public QWidget
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindowDisplaySettingDockWidgetView(QWidget *parent = 0);
	~VerticalCrossSectionWindowDisplaySettingDockWidgetView();

	bool showArbitrary() const;
	bool showBenchmark() const;
	bool showReferenceMark() const;
	bool showHub() const;
	bool showLeftHWMs() const;
	bool showRightHWMs() const;
	bool showStreamGage() const;

	bool showXSLine() const;
	bool showWSELine() const;
	bool showLeftHWMLine() const;
	bool showRightHWMLine() const;

private slots:
	void updateGraphicsView();

private:
	Ui::VerticalCrossSectionWindowDisplaySettingDockWidgetView *ui;
};

#endif // VERTICALCROSSSECTIONDISPLAYWINDOWSETTINGDOCKWIDGETVIEW_H
