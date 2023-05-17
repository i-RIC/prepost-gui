#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

class QSignalMapper;

class Post2dBirdEyeWindowGridShapeDataItem;
class Post2dBirdEyeWindowGridTypeDataItem;
class Post2dBirdEyeWindowCellScalarGroupTopDataItem;
class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;
class PostZoneDataContainer;

class Post2dBirdEyeWindowZoneDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, int zoneNumber, GraphicsWindowDataItem* parent);
	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	PostZoneDataContainer* dataContainer();
	int zoneNumber() const;
	std::string zoneName() const;

	void update();

	Post2dBirdEyeWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dBirdEyeWindowGridShapeDataItem* gridShapeDataItem() const;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* scalarGroupTopDataItem() const;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dBirdEyeWindowGridShapeDataItem* m_shapeDataItem;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;

	std::string m_zoneName;
	int m_zoneNumber;
};

#endif // POST2DWINDOWZONEDATAITEM_H
