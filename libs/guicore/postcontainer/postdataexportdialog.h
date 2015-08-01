#ifndef POSTDATAEXPORTDIALOG_H
#define POSTDATAEXPORTDIALOG_H

#include "../guicore_global.h"
#include "postexportsetting.h"

#include <QDialog>

namespace Ui
{
	class PostDataExportDialog;
}

class GUICOREDLL_EXPORT PostDataExportDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Format {VTKASCII, VTKBinary, CSV};
	explicit PostDataExportDialog(QWidget* parent = nullptr);
	~PostDataExportDialog();
	void setTimeValues(QList<double> timevalues);
	void setIJKRange(int inum, int jnum, int knum);

	void hideFormat();
	void hideDataRange();
	void setFileMode();

	void setFormat(Format f);
	void setExportSetting(const PostExportSetting& setting);
	void setPrefix(const QString& prefix);

	Format format() const;
	PostExportSetting exportSetting() const;
	QString prefix() const;

	void accept() override;
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
	Ui::PostDataExportDialog* ui;
	PostExportSetting m_setting;
};

#endif // POSTDATAEXPORTDIALOG_H
