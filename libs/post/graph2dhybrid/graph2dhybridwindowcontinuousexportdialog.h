#ifndef GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H
#define GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H

#include <QDialog>
#include "graph2dhybridwindowresultsetting.h"

namespace Ui
{
	class Graph2dHybridWindowContinuousExportDialog;
}

class Graph2dHybridWindowContinuousExportDialog : public QDialog
{
	Q_OBJECT

public:
	enum RegionMode {rmCurrentOnly, rmAll, rmCustom };
	enum TimeMode {tmCurrentOnly, tmAll, tmCustom };
	explicit Graph2dHybridWindowContinuousExportDialog(QWidget* parent = nullptr);
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
	void setLines(const std::vector<GeoDataPolyLineGroupPolyLine*>& lines);
	void setTimeMode(TimeMode tm);
	void setStartTimeStep(int startstep);
	void setEndTimeStep(int endstep);
	void setTimeSkip(int skip);

	QString folder() const;
	QString prefix() const;
	RegionMode regionMode() const;
	int iMin() const;
	int iMax() const;
	int jMin() const;
	int jMax() const;
	int kMin() const;
	int kMax() const;
	int indexMin() const;
	int indexMax() const;
	std::vector<GeoDataPolyLineGroupPolyLine*> lines() const;
	TimeMode timeMode() const;
	int startTimeStep() const;
	int endTimeStep() const;
	int timeSkip() const;

public slots:
	void accept() override;

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
	Ui::Graph2dHybridWindowContinuousExportDialog* ui;

	int m_i;
	int m_j;
	int m_k;
	int m_index;
	int m_currentStep;
	GeoDataPolyLineGroupPolyLine* m_currentLine;
	std::vector<GeoDataPolyLineGroupPolyLine*> m_lines;
};

#endif // GRAPH2DHYBRIDWINDOWCONTINUOUSEXPORTDIALOG_H
