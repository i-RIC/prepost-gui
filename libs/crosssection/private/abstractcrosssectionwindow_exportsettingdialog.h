#ifndef ABSTRACTCROSSSECTIONWINDOW_EXPORTSETTINGDIALOG_H
#define ABSTRACTCROSSSECTIONWINDOW_EXPORTSETTINGDIALOG_H

#include "../abstractcrosssectionwindow.h"

#include <QDialog>

namespace Ui {
class AbstractCrosssectionWindow_ExportSettingDialog;
}

class AbstractCrosssectionWindow::ExportSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum class CrossSectionMode {
		Current,
		All,
		Custom,
	};

	explicit ExportSettingDialog(AbstractCrosssectionWindow* window);
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
	AbstractCrosssectionWindow* m_window;
	Ui::AbstractCrosssectionWindow_ExportSettingDialog *ui;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_EXPORTSETTINGDIALOG_H
