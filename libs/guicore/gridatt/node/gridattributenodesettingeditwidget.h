#ifndef GRIDATTRIBUTENODESETTINGEDITWIDGET_H
#define GRIDATTRIBUTENODESETTINGEDITWIDGET_H

#include "../../guicore_global.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GridAttributeNodeSettingEditWidget;
}

class GridAttributeNodeSetting;

class GUICOREDLL_EXPORT GridAttributeNodeSettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit GridAttributeNodeSettingEditWidget(QWidget *parent);
	~GridAttributeNodeSettingEditWidget();

	GridAttributeNodeSetting setting() const;
	void setSetting(const GridAttributeNodeSetting& setting);
	void setSetting(GridAttributeNodeSetting* setting);

	void hideLineWidth();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	GridAttributeNodeSetting* m_setting;

	Ui::GridAttributeNodeSettingEditWidget *ui;
};

#endif // GRIDATTRIBUTENODESETTINGEDITWIDGET_H
