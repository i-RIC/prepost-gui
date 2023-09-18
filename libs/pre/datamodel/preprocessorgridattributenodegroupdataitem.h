#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/opacitycontainer.h>

#include <string>
#include <unordered_map>
#include <vector>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class PreProcessorGridAttributeNodeDataItem;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;
class QWidgetContainer;

class vtkActor;
class QAction;

class PreProcessorGridAttributeNodeGroupDataItem : public PreProcessorDataItem , public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeNodeGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void informDataChange(const std::string& name);
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informGridUpdate();
	std::vector<PreProcessorGridAttributeNodeDataItem*> conditions() const;
	PreProcessorGridAttributeNodeDataItem* nodeDataItem(const std::string& name) const;
	void handleStandardItemChange() override;
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

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

private:
	PreProcessorGridAttributeNodeDataItem* activeChildItem() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;

	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	std::string m_target;

	vtkActor* m_actor;

	QAction* m_showAttributeBrowserAction;
	OpacityContainer m_opacity;
	bool m_attributeBrowserFixed;
	std::unordered_map<std::string, PreProcessorGridAttributeNodeDataItem*> m_nameMap;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
