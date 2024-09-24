#ifndef VTKTEXTPROPERTYSETTINGWIDGET_H
#define VTKTEXTPROPERTYSETTINGWIDGET_H

#include "guibase_global.h"

#include <guibase/vtktextpropertysettingcontainer.h>
#include <misc/modifycommandwidget.h>

#include <QString>
#include <QWidget>

namespace Ui {
class vtkTextPropertySettingWidget;
}

class GUIBASEDLL_EXPORT vtkTextPropertySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit vtkTextPropertySettingWidget(QWidget *parent = nullptr);
	~vtkTextPropertySettingWidget();

	/// The setting
	vtkTextPropertySettingContainer setting() const;
	/// Set the setting
	void setSetting(const vtkTextPropertySettingContainer& setting);
	void setSetting(vtkTextPropertySettingContainer* setting);

	QUndoCommand* createModifyCommand(bool apply) override;

	/// Disable the widget to edit font size
	void disableSize();
	/// Hide the widget to edit font size
	void hideSize();
	/// Disable the widget to edit font color
	void disableColor();
	/// Hide the widget to edit font color
	void hideColor();

private:
	vtkTextPropertySettingContainer m_setting;
	vtkTextPropertySettingContainer* m_settingP;

	Ui::vtkTextPropertySettingWidget *ui;
};

#endif // VTKTEXTPROPERTYSETTINGWIDGET_H
