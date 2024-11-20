#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <misc/enumcontainert.h>
#include <misc/opacitycontainer.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class GridAttributeNodeSetting;
class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class PreProcessorGridAttributeNodeDataItem;
class PreProcessorGridDataItem;
class PreProcessorGridTypeDataItem;
class QWidgetContainer;

class QAction;

class PreProcessorGridAttributeNodeGroupDataItem : public PreProcessorDataItem , public TargetedItemI
{
	Q_OBJECT

public:
	PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* parent);
	~PreProcessorGridAttributeNodeGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

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
	GridAttributeNodeSetting& setting();
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

protected:
	void doUpdateActorSetting() override;

private:
	PreProcessorGridAttributeNodeDataItem* activeChildItem() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);
	PreProcessorGridTypeDataItem* gridTypeDataItem() const;

	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_H
