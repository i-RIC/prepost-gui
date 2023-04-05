#ifndef COLORMAPSETTINGEDITWIDGETWITHIMPORTEXPORTBUTTON_H
#define COLORMAPSETTINGEDITWIDGETWITHIMPORTEXPORTBUTTON_H

#include "../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class ColorMapSettingEditWidgetWithImportExportButton;
}

class ColorMapSettingEditWidgetI;

class GUICOREDLL_EXPORT ColorMapSettingEditWidgetWithImportExportButton : public ModifyCommandWidget
{
	Q_OBJECT

public:
	ColorMapSettingEditWidgetWithImportExportButton(ColorMapSettingEditWidgetI* widget, QWidget *parent);
	~ColorMapSettingEditWidgetWithImportExportButton();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	ColorMapSettingEditWidgetI* m_widget;
	Ui::ColorMapSettingEditWidgetWithImportExportButton *ui;
};

#endif // COLORMAPSETTINGEDITWIDGETWITHIMPORTEXPORTBUTTON_H
