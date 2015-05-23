#ifndef SCALARBARDIALOG_H
#define SCALARBARDIALOG_H

#include "guibase_global.h"
#include "scalarbarsetting.h"

#include <QDialog>

class vtkTextPropertySettingContainer;

namespace Ui
{
	class ScalarBarDialog;
}

/// Dialog to edit scalar bar setting
class GUIBASEDLL_EXPORT ScalarBarDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ScalarBarDialog(QWidget* parent = nullptr);
	~ScalarBarDialog();

	/// The setting
	const ScalarBarSetting setting();
	/// Set the setting
	void setSetting(const ScalarBarSetting& setting);
	/// The title of the scalar bar
	QString title() const;
	/// Set the title of the scalar bar
	void setTitle(const QString& title);
	/// The text setting of the title string
	const vtkTextPropertySettingContainer& titleTextSetting() const;
	/// Set the text setting of the title string
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	/// The text setting of the labels of scalar bar
	const vtkTextPropertySettingContainer& labelTextSetting() const;
	/// Set the text setting of the labels of scalar bar
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);

private:
	Ui::ScalarBarDialog* ui;
};

#endif // SCALARBARDIALOG_H
