#ifndef PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_H

#include "preprocessorgriddataitem.h"

#include <guicore/direction/directionsettingcontainer.h>
#include <guicore/named/namedgraphicwindowdataitem.h>
#include <guicore/pre/gridcond/base/gridattributevariationeditwidget.h>
#include <misc/boolcontainer.h>

class ColorMapSettingContainerI;
class ColorMapSettingToolBarWidgetController;
class PreProcessorGeoDataGroupDataItemI;
class PreProcessorGridAttributeAbstractCellGroupDataItem;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;
class SolverDefinitionGridAttribute;

class QAction;

class vtkActor;
class vtkMapper;
class vtkStructuredGrid;

class PreProcessorGridAttributeAbstractCellDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	PreProcessorGridAttributeAbstractCellDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataItem* parent);
	~PreProcessorGridAttributeAbstractCellDataItem();

	QDialog* propertyDialog(QWidget* parent) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void keyPressEvent(QKeyEvent*, VTKGraphicsView*) override;
	void keyReleaseEvent(QKeyEvent*, VTKGraphicsView*) override;
	void addCustomMenuItems(QMenu* menu) override;
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	SolverDefinitionGridAttribute* condition() const;
	void informDataChange();
	bool addToolBarButtons(QToolBar* toolbar) override;
	ColorMapSettingContainerI* colorMapSettingContainer() const;
	ColorMapSettingToolBarWidgetController* colorMapSettingToolBarWidgetController() const;
	void updateActorSetting() override;

public slots:
	void showPropertyDialog() override;

private slots:
	void editValue();
	void editDifference();
	void editRatio();
	void showDisplaySettingDialog();
	void exportToFile();
	void generatePointMap();

protected:
	PreProcessorGridAttributeAbstractCellGroupDataItem* groupDataItem() const;

	class PropertyDialog;

private:
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	PreProcessorGeoDataGroupDataItemI* geoDataGroupDataItem() const;
	PreProcessorGridDataItem* gridDataItem() const;
	void editVariation(GridAttributeVariationEditWidget::Mode mode, const QString& typeName);

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	int loadFromCgnsFile() override;
	void updateVisibility(bool visible) override;

	virtual PreProcessorGridDataItem::SelectedDataWithIdController* selectedDataController() const = 0;
	virtual QString positionCaption() const = 0;

	DirectionSettingContainer m_directionSetting;
	vtkActor* m_directionActor;
	SolverDefinitionGridAttribute* m_condition;

	BoolContainer m_isCustomModified;

	bool m_definingBoundingBox;
	QAction* m_editValueAction;
	QAction* m_exportAction;
	QAction* m_generatePointMapAction;
	QAction* m_editDifferenceAction;
	QAction* m_editRatioAction;

	ColorMapSettingToolBarWidgetController* m_colorMapToolBarWidgetController;
};

#endif // PREPROCESSORGRIDATTRIBUTEABSTRACTCELLDATAITEM_H
