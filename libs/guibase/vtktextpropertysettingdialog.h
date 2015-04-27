#ifndef VTKTEXTPROPERTYSETTINGDIALOG_H
#define VTKTEXTPROPERTYSETTINGDIALOG_H

#include "guibase_global.h"
#include <QDialog>
#include <QString>

class vtkTextPropertySettingContainer;

namespace Ui {
class vtkTextPropertySettingDialog;
}

class GUIBASEDLL_EXPORT vtkTextPropertySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit vtkTextPropertySettingDialog(QWidget *parent = nullptr);
	~vtkTextPropertySettingDialog();

	vtkTextPropertySettingContainer setting();
	void setSetting(const vtkTextPropertySettingContainer& setting);

	void disableSize();
	void disableColor();

private:
	QString m_prefix;
	Ui::vtkTextPropertySettingDialog *ui;
};

#endif // VTKTEXTPROPERTYSETTINGDIALOG_H
