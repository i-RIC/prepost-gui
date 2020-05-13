#ifndef VERTICALCROSSSECTIONWINDOW_H
#define VERTICALCROSSSECTIONWINDOW_H

#include "../../widgets/chartwindow.h"

#include <QWidget>

#include <vector>

class Project;
class RivmakerMainWindow;

class VerticalCrossSectionWindowDisplaySettingDockWidget;
class VerticalCrossSectionWindowElevationsDockWidget;
class VerticalCrossSectionWindowGraphicsView;

class QPushButton;

class VerticalCrossSectionWindow : public ChartWindow
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindow(RivmakerMainWindow *parent = nullptr);
	~VerticalCrossSectionWindow();

	Project* project() const;
	void setProject(Project* project);

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

	void updateGraphicsView();
	void exportWaterSurfaceElevation();

signals:
	void positionChangedForStatusBar(const QPointF& position);

private slots:
	void exportSVG();
	void exportCSV();
	void editDisplaySetting();
	void updateView();
	void setCsvExportEnabled(bool enabled);

private:
	void setupToolBars();
	void setupDockWidgets();

	void closeEvent(QCloseEvent *e);
	VerticalCrossSectionWindowGraphicsView* graphicsView() const;

	Project* m_project;

	QPushButton* m_exportSVGButton;
	QPushButton* m_exportCSVButton;

	QPushButton* m_displaySettingButton;

	VerticalCrossSectionWindowDisplaySettingDockWidget* m_displaySettingDockWidget;
	VerticalCrossSectionWindowElevationsDockWidget* m_elevationsDockWidget;

	RivmakerMainWindow* m_mainWindow;
};

#endif // VERTICALCROSSSECTIONWINDOW_H
