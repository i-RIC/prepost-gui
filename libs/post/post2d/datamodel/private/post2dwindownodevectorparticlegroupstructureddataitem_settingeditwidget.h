#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post2dwindownodevectorparticlegroupstructureddataitem_setting.h"
#include "../post2dwindownodevectorparticlegroupstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post2dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget;
}

class PostZoneDataContainer;

class Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dWindowNodeVectorParticleGroupStructuredDataItem* item, QWidget *parent = nullptr);
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
	Post2dWindowNodeVectorParticleGroupStructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post2dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
