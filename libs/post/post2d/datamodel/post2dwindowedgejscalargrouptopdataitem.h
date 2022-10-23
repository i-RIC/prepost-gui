#ifndef POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include "post2dwindowpointscalargrouptopdataitemi.h"

class Post2dWindowNodeScalarGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowEdgeJScalarGroupTopDataItem : public Post2dWindowDataItem, public Post2dWindowPointScalarGroupTopDataItemI
{
	Q_OBJECT
public:
	Post2dWindowEdgeJScalarGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowEdgeJScalarGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	Post2dWindowZoneDataItem* zoneDataItem() const override;
	bool hasTransparentPart() override;

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

	friend class Post2dWindowNodeScalarGroupDataItem;
};

#endif // POST2DWINDOWEDGEJSCALARGROUPTOPDATAITEM_H
