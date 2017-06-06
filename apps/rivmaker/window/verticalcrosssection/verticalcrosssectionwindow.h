#ifndef VERTICALCROSSSECTIONWINDOW_H
#define VERTICALCROSSSECTIONWINDOW_H

#include <QStandardItemModel>
#include <QWidget>

#include <vector>

class Project;
class RivmakerMainWindow;

class QwtPlotCurve;
class QwtPlotMarker;

namespace Ui {
class VerticalCrossSectionWindow;
}

class VerticalCrossSectionWindow : public QWidget
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindow(RivmakerMainWindow *parent);
	~VerticalCrossSectionWindow();

	void setProject(Project* project);

private slots:
	void updateView();
	void handleTableEdit(QStandardItem* editedItem);
	void exportWaterSurfaceElevation();

private:
	void initPlot();
	void initTable();

	void updatePlot();
	void updateTable();

	void setupCrossSectionLine();
	void setupCrossSectionMarkers(double *xmin, double *xmax, bool *first);

	void updateScale(double xmin, double xmax, double ymin, double ymax);

	void closeEvent(QCloseEvent *e);

	QwtPlotCurve* m_csCurve;

	QwtPlotCurve* m_arbitraryCurve;
	QwtPlotCurve* m_benchmarkCurve;
	QwtPlotCurve* m_referenceMarkCurve;
	QwtPlotCurve* m_hubCurve;
	QwtPlotCurve* m_leftBankCurve;
	QwtPlotCurve* m_rightBankCurve;
	QwtPlotCurve* m_streamGageCurve;

	std::vector<QwtPlotMarker*> m_crossSectionMarkers;

	QStandardItemModel m_tableModel;

	Project* m_project;
	RivmakerMainWindow* m_mainWindow;
	Ui::VerticalCrossSectionWindow *ui;
};

#endif // VERTICALCROSSSECTIONWINDOW_H
