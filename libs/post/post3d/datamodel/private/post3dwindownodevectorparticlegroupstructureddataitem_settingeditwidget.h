#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post3dwindownodevectorparticlegroupstructureddataitem_setting.h"
#include "../post3dwindownodevectorparticlegroupstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget;
}

class PostZoneDataContainer;

class Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post3dWindowNodeVectorParticleGroupStructuredDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;
	Setting setting();
	void setSetting(const Setting& setting);

	class StartPositionEditWidget;

private slots:
	void addStartPosition();
	void removeStartPosition();
	void handleCurrentStartPositionChanged(int index);

private:
	void updateRemoveButtonStatus();
	void updateStartPositionList();
	void applyStartPosition(int index);

	int m_current;
	std::vector<Setting::StartPosition> m_startPositions;
	Post3dWindowNodeVectorParticleGroupStructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
