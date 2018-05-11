#ifndef VTKTEXTPROPERTYSETTINGDIALOG_H
#define VTKTEXTPROPERTYSETTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>
#include <QString>

class vtkTextPropertySettingContainer;

namespace Ui
{
	class vtkTextPropertySettingDialog;
}

/// Dialog to select vtkTextPropertySettingContainer values
class GUIBASEDLL_EXPORT vtkTextPropertySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit vtkTextPropertySettingDialog(QWidget* parent = nullptr);
	~vtkTextPropertySettingDialog();
	/// The setting
	vtkTextPropertySettingContainer setting() const;
	/// Set the setting
	void setSetting(const vtkTextPropertySettingContainer& setting);

	/// Disable the widget to edit font size
	void disableSize();
	/// Disable the widget to edit font color
	void disableColor();

private:
	QString m_prefix;
	Ui::vtkTextPropertySettingDialog* ui;
};

#endif // VTKTEXTPROPERTYSETTINGDIALOG_H
