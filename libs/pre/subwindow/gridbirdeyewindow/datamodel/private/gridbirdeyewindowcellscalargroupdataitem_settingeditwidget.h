#ifndef GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../gridbirdeyewindowcellscalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GridBirdEyeWindowCellScalarGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidgetI;

class GridBirdEyeWindowCellScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(GridBirdEyeWindowCellScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

	Setting setting() const;
	void setSetting(const Setting& setting);
	void setSetting(Setting* setting);

private slots:
	void handleNodeRadioButtonClick();
	void handleCellRadioButtonClick();
	void handleNodeScalarChange(int index);
	void handleCellScalarChange(int index);

private:
	Setting* m_setting;
	std::vector<std::string> m_nodeValueNames;
	std::vector<std::string> m_cellValueNames;

	ColorMapSettingEditWidgetI* m_colorMapEditWidget;
	QWidget* m_colorMapEditWidgetWithImportExport;
	GridBirdEyeWindowCellScalarGroupDataItem* m_item;

	Ui::GridBirdEyeWindowCellScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
