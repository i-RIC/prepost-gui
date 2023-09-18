#ifndef POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class ColorMapSettingContainerI;
class ColorMapSettingToolBarWidgetController;
class Post2dWindowGeoDataGroupDataItem;
class Post2dWindowGridAttributeNodeGroupDataItem;
class Post2dWindowInputGridDataItem;
class Post2dWindowGridTypeDataItem;
class SolverDefinitionGridAttribute;

class Post2dWindowGridAttributeNodeDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeNodeDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~Post2dWindowGridAttributeNodeDataItem();

	QDialog* propertyDialog(QWidget* parent) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	SolverDefinitionGridAttribute* condition() const;
	ColorMapSettingContainerI* colorMapSettingContainer() const;
	ColorMapSettingToolBarWidgetController* colorMapSettingToolBarWidgetController() const;

	bool addToolBarButtons(QToolBar* toolbar) override;

public slots:
	void showPropertyDialog() override;

private:
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowGeoDataGroupDataItem* geoDataGroupDataItem() const;
	Post2dWindowGridAttributeNodeGroupDataItem* groupDataItem() const;
	Post2dWindowInputGridDataItem* gridDataItem() const;

	void doApplyOffset(double x, double y) override;
	void updateVisibility(bool visible) override;

	SolverDefinitionGridAttribute* m_condition;

	ColorMapSettingToolBarWidgetController* m_colorMapToolBarWidgetController;

	class PropertyDialog;
};

#endif // POST2DWINDOWGRIDATTRIBUTENODEDATAITEM_H
