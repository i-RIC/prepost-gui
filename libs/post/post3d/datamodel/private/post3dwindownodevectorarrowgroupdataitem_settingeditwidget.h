#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindownodevectorarrowgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowNodeVectorArrowGroupDataItem_SettingEditWidget;
}

class Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowNodeVectorArrowGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply);

private slots:
	void addFace();
	void removeFace();
	void setCurrentFace(int row);
	void updateImageSetting();
	void updateColorMapImageSetting();

private:
	void updateFaceList();
	void saveCurrentFace();
	void clearWidgets();

	ArrowsSettingContainer m_setting;
	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> m_settings;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	Post3dWindowNodeVectorArrowGroupDataItem* m_item;

	int m_currentRow;
	int m_gridDimensions[3];

	Ui::Post3dWindowNodeVectorArrowGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETTINGEDITWIDGET_H
