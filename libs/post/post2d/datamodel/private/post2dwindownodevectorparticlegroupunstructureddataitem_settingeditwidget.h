#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post2dwindownodevectorparticlegroupunstructureddataitem_setting.h"
#include "../post2dwindownodevectorparticlegroupunstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget;
}

class PostZoneDataContainer;

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;
	Setting setting();
	void setSetting(const Setting& setting);

	class StartPositionEditWidget;

public slots:
	void handleMousePoint1Change(const QPointF point);
	void handleMousePoint2Change(const QPointF point);

private slots:
	void addStartPosition();
	void removeStartPosition();
	void handleCurrentStartPositionChanged(int index);

private:
	void setupSolutionComboBox(PostZoneDataContainer* data);
	void updateRemoveButtonStatus();
	void updateStartPositionList();
	void applyStartPosition(int index);

	int m_current;
	std::vector<Setting::StartPosition> m_startPositions;
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
