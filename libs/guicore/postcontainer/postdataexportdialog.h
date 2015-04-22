#ifndef POSTDATAEXPORTDIALOG_H
#define POSTDATAEXPORTDIALOG_H

#include "../guicore_global.h"
#include <QDialog>
#include <QPushButton>

namespace Ui {
		class PostDataExportDialog;
}

class GUICOREDLL_EXPORT PostDataExportDialog : public QDialog
{
		Q_OBJECT

public:
	enum Format {fmVTK, fmCSV};
	explicit PostDataExportDialog(QWidget *parent = nullptr);
	~PostDataExportDialog();
	void setTimeValues(QList<double> timevalues);
	void setIJKRange(int inum, int jnum, int knum);

	void hideFormat();
	void hideDataRange();
	void setFileMode();

	void setFormat(Format f);
	void setOutputFolder(const QString& folder);
	void setOutputFileName(const QString& filename);
	void setAllTimeSteps(bool all);
	void setStartTimeStep(int start);
	void setEndTimeStep(int end);
	void setPrefix(const QString& prefix);
	void setSkipRate(int rate);

	void setFullRange(bool full);
	void setIMin(int imin);
	void setIMax(int imax);
	void setJMin(int jmin);
	void setJMax(int jmax);
	void setKMin(int kmin);
	void setKMax(int kmax);

	Format format();
	const QString outputFolder() const;
	const QString outputFileName() const;
	bool allTimeSteps();
	int startTimeStep();
	int endTimeStep();
	bool fullRange();
	int iMin();
	int iMax();
	int jMin();
	int jMax();
	int kMin();
	int kMax();

	const QString prefix();
	int skipRate();
	void accept();
	void updateSkipRateMaximum();

private slots:
	void allCheckChange(bool checked);
	void handleStartChange(int val);
	void handleEndChange(int val);
	void fullRegionChange(bool checked);
	void handleIMinChange(int val);
	void handleIMaxChange(int val);
	void handleJMinChange(int val);
	void handleJMaxChange(int val);
	void handleKMinChange(int val);
	void handleKMaxChange(int val);
	void handleRefButtonClick();
	void toggleRangeGroupBox();

private:
		Ui::PostDataExportDialog *ui;
};

#endif // POSTDATAEXPORTDIALOG_H
