#ifndef POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <misc/opacitycontainer.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class Post2dWindowGridAttributeNodeDataItem;
class Post2dWindowInputGridDataItem;
class Post2dWindowGridTypeDataItem;
class QWidgetContainer;

class vtkActor;
class QAction;

class Post2dWindowGridAttributeNodeGroupDataItem : public Post2dWindowDataItem , public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeNodeGroupDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowGridAttributeNodeGroupDataItem();

	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void updateZDepthRangeItemCount() override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;

	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	std::vector<Post2dWindowGridAttributeNodeDataItem*> conditions() const;
	Post2dWindowGridAttributeNodeDataItem* nodeDataItem(const std::string& name) const;
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
	Post2dWindowInputGridDataItem* gridDataItem() const;
	bool colorBarShouldBeVisible(const std::string& name) const;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

private:
	Post2dWindowGridAttributeNodeDataItem* activeChildItem() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* v);
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindow* post2dWindow() const;

	vtkIdType findVertex(const QPoint& p, VTKGraphicsView* v);

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_H
