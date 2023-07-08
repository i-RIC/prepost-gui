#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dbirdeyewindownodescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidgetI;

class Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent);
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
	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;

	Ui::Post2dBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
