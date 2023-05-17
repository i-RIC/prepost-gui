#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dbirdeyewindowcellscalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dBirdEyeWindowCellScalarGroupDataItem_SettingEditWidget;
}

class ColorMapSettingEditWidget;

class Post2dBirdEyeWindowCellScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dBirdEyeWindowCellScalarGroupDataItem* item, QWidget *parent);
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

	ColorMapSettingEditWidget* m_colorMapEditWidget;
	Post2dBirdEyeWindowCellScalarGroupDataItem* m_item;

	Ui::Post2dBirdEyeWindowCellScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
