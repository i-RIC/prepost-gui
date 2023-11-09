#ifndef POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include <misc/boolcontainer.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditwidget.h>

class ColorMapSettingContainerI;
class ColorMapSettingToolBarWidgetController;
class Post2dWindowGeoDataGroupDataItem;
class Post2dWindowGridAttributeAbstractCellGroupDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowInputGridDataItem;
class SolverDefinitionGridAttribute;

class Post2dWindowGridAttributeAbstractCellDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeAbstractCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~Post2dWindowGridAttributeAbstractCellDataItem();

	QDialog* propertyDialog(QWidget* parent) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	SolverDefinitionGridAttribute* condition() const;
	bool addToolBarButtons(QToolBar* toolbar) override;
	ColorMapSettingContainerI* colorMapSettingContainer() const;
	ColorMapSettingToolBarWidgetController* colorMapSettingToolBarWidgetController() const;

public slots:
	void showPropertyDialog() override;

protected:
	Post2dWindowGridAttributeAbstractCellGroupDataItem* groupDataItem() const;

	class PropertyDialog;

private:
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowGeoDataGroupDataItem* geoDataGroupDataItem() const;
	Post2dWindowInputGridDataItem* gridDataItem() const;

	void updateVisibility(bool visible) override;

	virtual QString positionCaption() const = 0;

	SolverDefinitionGridAttribute* m_condition;

	ColorMapSettingToolBarWidgetController* m_colorMapToolBarWidgetController;
};

#endif // POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLDATAITEM_H
