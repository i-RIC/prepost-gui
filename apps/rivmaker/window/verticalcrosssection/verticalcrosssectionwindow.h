#ifndef VERTICALCROSSSECTIONWINDOW_H
#define VERTICALCROSSSECTIONWINDOW_H

#include <QWidget>

#include <vector>

class Project;

class QwtPlotCurve;
class QwtPlotMarker;

namespace Ui {
class VerticalCrossSectionWindow;
}

class VerticalCrossSectionWindow : public QWidget
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindow(QWidget *parent = 0);
	~VerticalCrossSectionWindow();

	void setProject(Project* project);

private slots:
	void updateView();

private:
	void initPlot();
	void initTable();
	void setupCrossSectionMarkers(double *xmin, double *xmax, bool *first);

	void updateScale(double xmin, double xmax, double ymin, double ymax);

	void closeEvent(QCloseEvent *e);

	QwtPlotCurve* m_arbitraryCurve;
	QwtPlotCurve* m_leftBankCurve;
	QwtPlotCurve* m_rightBankCurve;

	std::vector<QwtPlotMarker*> m_crossSectionMarkers;

	Project* m_project;
	Ui::VerticalCrossSectionWindow *ui;
};

#endif // VERTICALCROSSSECTIONWINDOW_H
