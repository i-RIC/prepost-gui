#ifndef GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../gridbirdeyewindownodescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class GridBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidgetI;

class GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(GridBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent);
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
	GridBirdEyeWindowNodeScalarGroupDataItem* m_item;

	Ui::GridBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
