#ifndef POST2DWINDOWZONEDATAITEM_H
#define POST2DWINDOWZONEDATAITEM_H

#include "../gridbirdeyewindowdataitem.h"

class Grid;
class GridBirdEyeWindowGridShapeDataItem;
class GridBirdEyeWindowGridTypeDataItem;
class GridBirdEyeWindowCellScalarGroupTopDataItem;
class GridBirdEyeWindowNodeScalarGroupTopDataItem;
class PreProcessorGridTypeDataItem;

class GridBirdEyeWindowZoneDataItem : public GridBirdEyeWindowDataItem
{
	Q_OBJECT

public:
	GridBirdEyeWindowZoneDataItem(GraphicsWindowDataItem* parent);
	void addCustomMenuItems(QMenu* menu) override;

	// Standard mouse event handlers
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	void update();

	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	Grid* grid();
	GridBirdEyeWindowGridShapeDataItem* gridShapeDataItem() const;
	GridBirdEyeWindowNodeScalarGroupTopDataItem* scalarGroupTopDataItem() const;
	GridBirdEyeWindowCellScalarGroupTopDataItem* cellScalarGroupTopDataItem() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GridBirdEyeWindowGridShapeDataItem* m_shapeDataItem;
	GridBirdEyeWindowNodeScalarGroupTopDataItem* m_scalarGroupTopDataItem;
	GridBirdEyeWindowCellScalarGroupTopDataItem* m_cellScalarGroupTopDataItem;
};

#endif // POST2DWINDOWZONEDATAITEM_H
