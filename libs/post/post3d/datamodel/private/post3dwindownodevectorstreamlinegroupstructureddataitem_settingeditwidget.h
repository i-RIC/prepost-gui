#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "post3dwindownodevectorstreamlinegroupstructureddataitem_setting.h"
#include "../post3dwindownodevectorstreamlinegroupstructureddataitem.h"
#include <misc/modifycommandwidget.h>

#include <QWidget>

#include <string>
#include <vector>

namespace Ui {
class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget;
}

class PostZoneDataContainer;

class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit SettingEditWidget(Post3dWindowNodeVectorStreamlineGroupStructuredDataItem* item, QWidget *parent = nullptr);
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
	void setupSolutionComboBox(PostZoneDataContainer* data);
	void updateRemoveButtonStatus();
	void updateStartPositionList();
	void applyStartPosition(int index);

	int m_current;
	std::vector<Setting::StartPosition> m_startPositions;
	Post3dWindowNodeVectorStreamlineGroupStructuredDataItem* m_item;
	std::vector<std::string> m_solutions;

	Ui::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
