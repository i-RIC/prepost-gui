#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <memory>

class Post2dBirdEyeWindowGridShapeDataItem;
class Post2dBirdEyeWindowGridTypeDataItem;
class Post2dBirdEyeWindowCellScalarGroupTopDataItem;
class Post2dBirdEyeWindowNodeScalarGroupTopDataItem;
class v4PostZoneDataContainer;

class Post2dBirdEyeWindowZoneDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	Post2dBirdEyeWindowZoneDataItem(const std::string& zoneName, GraphicsWindowDataItem* parent);
	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	v4PostZoneDataContainer* v4DataContainer();
	std::string zoneName() const;

	void update();

	Post2dBirdEyeWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dBirdEyeWindowGridShapeDataItem* gridShapeDataItem() const;
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* scalarGroupTopDataItem() const;
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWZONEDATAITEM_H
