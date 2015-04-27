#ifndef SCALARBARDIALOG_H
#define SCALARBARDIALOG_H

#include "guibase_global.h"
#include "scalarbarsetting.h"

#include <QDialog>

class vtkTextPropertySettingContainer;

namespace Ui {
	class ScalarBarDialog;
}

class GUIBASEDLL_EXPORT ScalarBarDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ScalarBarDialog(QWidget *parent = nullptr);
	~ScalarBarDialog();

	void setSetting(const ScalarBarSetting& setting);
	void setTitle(const QString& title);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);

	const ScalarBarSetting setting();
	QString title();
	const vtkTextPropertySettingContainer& titleTextSetting() const;
	const vtkTextPropertySettingContainer& labelTextSetting() const;

private:
	Ui::ScalarBarDialog *ui;
};

#endif // SCALARBARDIALOG_H
