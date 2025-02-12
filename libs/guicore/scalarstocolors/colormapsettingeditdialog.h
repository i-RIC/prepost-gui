#ifndef COLORMAPSETTINGEDITDIALOG_H
#define COLORMAPSETTINGEDITDIALOG_H

#include "../guicore_global.h"

#include <QDialog>

namespace Ui {
class ColorMapSettingEditDialog;
}

class ColorMapSettingContainerI;
class ColorMapSettingEditWidgetI;
class ProjectDefaultColorMapSettings;

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
	void setupSetAsDefaultButton(const std::string& name, ProjectDefaultColorMapSettings* settings);

	QUndoCommand* createModifyCommand() const;

private slots:
	void setAsDefault();

private:
	ColorMapSettingEditWidgetI* m_widget;

	std::string m_targetName;
	ProjectDefaultColorMapSettings* m_defaultColorMapSettings;

	Ui::ColorMapSettingEditDialog *ui;
};

#endif // COLORMAPSETTINGEDITDIALOG_H
