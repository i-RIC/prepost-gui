#ifndef POST2DWINDOWNODEVECTORARROWGROUPTOPDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowNodeVectorArrowGroupDataItem;

class Post2dWindowNodeVectorArrowGroupTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowNodeVectorArrowGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorArrowGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void update();

	Post2dWindowZoneDataItem* zoneDataItem() const;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

private:
	void addCustomMenuItems(QMenu* menu) override;

	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* propDialog) override;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowNodeVectorArrowGroupDataItem* buildItem(const std::string& name);
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPTOPDATAITEM_H
