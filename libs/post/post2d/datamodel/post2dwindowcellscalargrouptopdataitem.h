#ifndef POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H
#define POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

class Post2dWindowCellScalarGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowCellScalarGroupTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	Post2dWindowCellScalarGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowCellScalarGroupTopDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	Post2dWindowZoneDataItem* zoneDataItem() const;
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

	friend class Post2dWindowCellScalarGroupDataItem;
};

#endif // POST2DWINDOWCELLSCALARGROUPTOPDATAITEM_H
