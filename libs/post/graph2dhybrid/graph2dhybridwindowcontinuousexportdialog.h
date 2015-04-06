#ifndef GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H
#define GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H

#include <QDialog>
#include "graph2dhybridwindowresultsetting.h"

namespace Ui {
    class Graph2dHybridWindowContinuousExportDialog;
}

class Graph2dHybridWindowContinuousExportDialog : public QDialog
{
    Q_OBJECT

public:
	enum RegionMode{rmCurrentOnly, rmAll, rmCustom };
	enum TimeMode {tmCurrentOnly, tmAll, tmCustom };
    explicit Graph2dHybridWindowContinuousExportDialog(QWidget *parent = 0);
    ~Graph2dHybridWindowContinuousExportDialog();
	void setSetting(const Graph2dHybridWindowResultSetting& setting, int dim[4]);
	void setTimesteps(QList<double> steps);
	void setCurrentStep(int step);

	void setFolder(const QString& folder);
	void setPrefix(const QString& prefix);
	void setRegionMode(RegionMode rm);
	void setIMin(int imin);
	void setIMax(int imax);
	void setJMin(int jmin);
	void setJMax(int jmax);
	void setKMin(int kmin);
	void setKMax(int kmax);
	void setIndexMin(int idxmin);
	void setIndexMax(int idxmax);
	void setTimeMode(TimeMode tm);
	void setStartTimeStep(int startstep);
	void setEndTimeStep(int endstep);
	void setTimeSkip(int skip);

	const QString folder();
	const QString prefix();
	RegionMode regionMode();
	int iMin();
	int iMax();
	int jMin();
	int jMax();
	int kMin();
	int kMax();
	int indexMin();
	int indexMax();
	TimeMode timeMode();
	int startTimeStep();
	int endTimeStep();
	int timeSkip();
public slots:
	void accept();
private slots:
	void regionCurrentToggled(bool toggled);
	void fullRegionToggled(bool toggled);

	void iMinChanged(int imin);
	void iMaxChanged(int imax);
	void jMinChanged(int imin);
	void jMaxChanged(int imax);
	void kMinChanged(int imin);
	void kMaxChanged(int imax);
	void indexMinChanged(int imin);
	void indexMaxChanged(int imax);

	void timeCurrentToggled(bool toggled);
	void timeAllToggled(bool toggled);

	void timeStartChanged(int start);
	void timeEndChanged(int end);
private:
    Ui::Graph2dHybridWindowContinuousExportDialog *ui;

	int m_i;
	int m_j;
	int m_k;
	int m_index;
	int m_currentStep;
};

#endif // GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H
