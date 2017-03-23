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

class CrossSectionWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CrossSectionWindow(QWidget *parent = 0);
	~CrossSectionWindow();

	void setProject(Project* project);

private slots:
	void updateView();
	void handleSelectionChange(int selected);

private:
	void initCurve();
	void updateCurve();

	Ui::CrossSectionWindow *ui;

	QwtPlotCurve* m_curve;
	QwtPlotMarker* m_waterElevationMarker;

	CrossSection* m_currentCrossSection;
	Project* m_project;
};

#endif // CROSSSECTIONWINDOW_H
