#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post2dwindownodevectorstreamlinegroupstructureddataitem_setting.h"
#include "../post2dwindownodevectorstreamlinegroupstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget;
}

class PostZoneDataContainer;

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* item, QWidget *parent = nullptr);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;
	Setting setting() const;
	void setSetting(const Setting& setting);

	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting baseSetting() const;
	void setBaseSetting(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& setting);

	class StartPositionEditWidget;

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
	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
