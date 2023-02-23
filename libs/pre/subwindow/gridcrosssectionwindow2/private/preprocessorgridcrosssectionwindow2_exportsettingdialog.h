#ifndef PREPROCESSORGRIDCROSSSECTIONWINDOW2_EXPORTSETTINGDIALOG_H
#define PREPROCESSORGRIDCROSSSECTIONWINDOW2_EXPORTSETTINGDIALOG_H

#include "../preprocessorgridcrosssectionwindow2.h"

#include <QDialog>

namespace Ui {
class PreProcessorGridCrosssectionWindow2_ExportSettingDialog;
}

class PreProcessorGridCrosssectionWindow2::ExportSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum class CrossSectionMode {
		Current,
		All,
		Custom,
	};

	explicit ExportSettingDialog(PreProcessorGridCrosssectionWindow2* window);
	~ExportSettingDialog();

	QString dirName() const;
	void setDirName(const QString& dirName);

	QString prefix() const;
	void setPrefix(const QString& prefix);

	CrossSectionMode mode() const;
	void setMode(CrossSectionMode mode);

	int min() const;
	void setMin(int min);

	int max() const;
	void setMax(int max);

	int skipRate() const;
	void setSkipRate(int skipRate);

public slots:
	void accept() override;

private slots:
	void handleCurrentClicked();
	void handleAllClicked();

private:
	PreProcessorGridCrosssectionWindow2* m_window;
	Ui::PreProcessorGridCrosssectionWindow2_ExportSettingDialog *ui;
};

#endif // PREPROCESSORGRIDCROSSSECTIONWINDOW2_EXPORTSETTINGDIALOG_H
