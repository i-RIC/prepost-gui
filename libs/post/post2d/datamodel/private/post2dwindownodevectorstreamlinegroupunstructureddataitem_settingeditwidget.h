#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post2dwindownodevectorstreamlinegroupunstructureddataitem_setting.h"
#include "../post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem_SettingEditWidget;
}

class v4PostZoneDataContainer;

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;
	Setting setting();
	void setSetting(const Setting& setting);

	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting baseSetting() const;
	void setBaseSetting(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& setting);

	class StartPositionEditWidget;

public slots:
	void handleMousePoint1Change(const QPointF point);
	void handleMousePoint2Change(const QPointF point);

private slots:
	void addStartPosition();
	void removeStartPosition();
	void handleCurrentStartPositionChanged(int index);

private:
	void setupSolutionComboBox(v4PostZoneDataContainer* data);
	void updateRemoveButtonStatus();
	void updateStartPositionList();
	void applyStartPosition(int index);

	int m_current;
	std::vector<Setting::StartPosition> m_startPositions;
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
