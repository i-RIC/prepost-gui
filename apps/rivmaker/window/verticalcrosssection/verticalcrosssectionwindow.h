#ifndef VERTICALCROSSSECTIONWINDOW_H
#define VERTICALCROSSSECTIONWINDOW_H

#include <QWidget>

class Project;

class QwtPlotCurve;

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
	void updateScale(double xmin, double xmax, double ymin, double ymax);

	void closeEvent(QCloseEvent *e);

	QwtPlotCurve* m_arbitraryCurve;
	QwtPlotCurve* m_leftBankCurve;
	QwtPlotCurve* m_rightBankCurve;

	Project* m_project;
	Ui::VerticalCrossSectionWindow *ui;
};

#endif // VERTICALCROSSSECTIONWINDOW_H
