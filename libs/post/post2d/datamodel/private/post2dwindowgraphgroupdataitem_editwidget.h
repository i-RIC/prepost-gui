#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_H

#include "../post2dwindowgraphsetting.h"
#include "../post2dwindowgraphgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowGraphGroupDataItem_EditWidget;
}

class Post2dWindowGraphGroupDataItem::EditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	EditWidget(Post2dWindowGraphGroupDataItem* item, QWidget *parent);
	~EditWidget();

	void setDimensions(int dimI, int dimJ);
	void setTargets(const std::vector<std::string>& targets);

	Post2dWindowGraphSetting setting() const;
	void setSetting(const Post2dWindowGraphSetting& setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private slots:
	void handleDirChange(bool checked);
	void handleStartChange(int val);
	void handleEndChange(int val);

	void editRegionCustomSetting();

private:
	void updateStartEndValueRange();
	void resetRegionCustomSetting();

	int m_dimI;
	int m_dimJ;

	Post2dWindowGraphSetting m_setting;
	Post2dWindowGraphGroupDataItem* m_item;

	Ui::Post2dWindowGraphGroupDataItem_EditWidget *ui;

	class CustomRegionDialog;
};

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_EDITWIDGET_H
