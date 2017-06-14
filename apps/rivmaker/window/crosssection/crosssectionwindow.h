#ifndef CROSSSECTIONWINDOW_H
#define CROSSSECTIONWINDOW_H

#include <QMainWindow>

namespace Ui {
class CrossSectionWindow;
}

class CrossSection;
class Project;

class QwtPlotCurve;
class QwtPlotMarker;
class QwtPlotZoomer;

class CrossSectionWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CrossSectionWindow(QWidget *parent = 0);
	~CrossSectionWindow();

	void setProject(Project* project);

signals:
	void positionChangedForStatusBar(const QPointF& position);

private slots:
	void resetZoom();

	void updateView();
	void handleSelectionChange(int selected);

private:
	void initCurve();
	void updateCurve();
	void updateHWMs();
	void updateWindowTitle();

	Ui::CrossSectionWindow *ui;

	QwtPlotCurve* m_curve;
	QwtPlotMarker* m_waterElevationMarker;

	QwtPlotMarker* m_lbHWM;
	QwtPlotMarker* m_rbHWM;

	QwtPlotZoomer* m_zoomer;

	CrossSection* m_currentCrossSection;
	Project* m_project;
};

#endif // CROSSSECTIONWINDOW_H
