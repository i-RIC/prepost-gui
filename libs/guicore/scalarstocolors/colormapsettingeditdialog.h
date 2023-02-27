#ifndef COLORMAPSETTINGEDITDIALOG_H
#define COLORMAPSETTINGEDITDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

namespace Ui {
class ColorMapSettingEditDialog;
}

class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;

class GUICOREDLL_EXPORT ColorMapSettingEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ColorMapSettingEditDialog(QWidget *parent = nullptr);
	~ColorMapSettingEditDialog();

	void disableApplyButton();

	ColorMapSettingEditWidgetI* widget();
	void setWidget(ColorMapSettingEditWidgetI* w);

	ColorMapSettingContainerI* setting() const;
	void setSetting(ColorMapSettingContainerI* setting);

	QUndoCommand* createModifyCommand() const;

private:
	ColorMapSettingEditWidgetI* m_widget;
	Ui::ColorMapSettingEditDialog *ui;
};

#endif // COLORMAPSETTINGEDITDIALOG_H
