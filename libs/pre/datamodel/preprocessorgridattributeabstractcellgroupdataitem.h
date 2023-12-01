#ifndef PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/opacitycontainer.h>

#include <unordered_map>
#include <vector>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class PreProcessorGridAttributeAbstractCellDataItem;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;
class QWidgetContainer;
class vtkPointSetExtended;

class vtkActor;
class QAction;

class PreProcessorGridAttributeAbstractCellGroupDataItem : public PreProcessorDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeAbstractCellGroupDataItem(const QString& caption, PreProcessorDataItem* parent);
	~PreProcessorGridAttributeAbstractCellGroupDataItem();

	void setupChildren();
	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void informDataChange(const std::string& name);
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void informGridUpdate();
	std::vector<PreProcessorGridAttributeAbstractCellDataItem*> conditions() const;
	PreProcessorGridAttributeAbstractCellDataItem* cellDataItem(const std::string& name) const;
	void handleStandardItemChange() override;
	IntContainer& lineWidth();
	OpacityContainer& opacity();
	OpacityContainerWidget* opacityWidget() const;
	QWidgetContainer* colorMapWidgetContainer() const;
	QAction* showAttributeBrowserAction() const;
	void addCustomMenuItems(QMenu* menu) override;
	void initAttributeBrowser();
	void clearAttributeBrowser();
	void fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	void updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v);
	bool addToolBarButtons(QToolBar* toolbar) override;
	void applyColorMapSetting(const std::string& name);
	PreProcessorGridDataItem* gridDataItem() const;
	bool colorBarShouldBeVisible(const std::string& name) const;

	virtual vtkPointSetExtended* data() const = 0;
	virtual vtkPointSet* filteredData() const = 0;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

private:
	PreProcessorGridAttributeAbstractCellDataItem* activeChildItem() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;
	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);

	virtual SolverDefinitionGridAttribute::Position definitionPosition() const = 0;
	virtual PreProcessorGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) = 0;
	virtual void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const = 0;

	std::string m_target;
	vtkActor* m_actor;
	std::unordered_map<std::string, PreProcessorGridAttributeAbstractCellDataItem*> m_nameMap;

	IntContainer m_lineWidth;
	OpacityContainer m_opacity;
	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H
