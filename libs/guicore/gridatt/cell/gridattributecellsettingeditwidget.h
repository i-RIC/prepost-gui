#ifndef GRIDATTRIBUTECELLSETTINGEDITWIDGET_H
#define GRIDATTRIBUTECELLSETTINGEDITWIDGET_H

#include "../../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GridAttributeCellSettingEditWidget;
}

class GridAttributeCellSetting;

class GUICOREDLL_EXPORT GridAttributeCellSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit GridAttributeCellSettingEditWidget(QWidget *parent);
	~GridAttributeCellSettingEditWidget();

	GridAttributeCellSetting setting() const;
	void setSetting(const GridAttributeCellSetting& setting);
	void setSetting(GridAttributeCellSetting* setting);

	void hideLineWidth();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	GridAttributeCellSetting* m_setting;

	Ui::GridAttributeCellSettingEditWidget *ui;
};

#endif // GRIDATTRIBUTECELLSETTINGEDITWIDGET_H
