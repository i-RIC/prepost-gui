#ifndef POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H
#define POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <misc/opacitycontainer.h>

#include <memory>
#include <unordered_map>
#include <vector>

class NamedGraphicWindowDataItem;
class OpacityContainerWidget;
class Post2dWindowGridAttributeAbstractCellDataItem;
class Post2dWindowGridTypeDataItem;
class Post2dWindowInputGridDataItem;
class QWidgetContainer;
class vtkPointSetExtended;

class vtkActor;
class QAction;

class Post2dWindowGridAttributeAbstractCellGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowGridAttributeAbstractCellGroupDataItem(const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowGridAttributeAbstractCellGroupDataItem();

	void setupChildren();
	std::string target() const override;
	void setTarget(const std::string& target) override;

	void updateActorSetting() override;
	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;

	QDialog* propertyDialog(QWidget* parent) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void update();
	std::vector<Post2dWindowGridAttributeAbstractCellDataItem*> conditions() const;
	Post2dWindowGridAttributeAbstractCellDataItem* cellDataItem(const std::string& name) const;
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
	Post2dWindowInputGridDataItem* gridDataItem() const;
	bool colorBarShouldBeVisible(const std::string& name) const;

	virtual vtkPointSetExtended* data() const = 0;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);
	void showAttributeBrowser();

private:
	Post2dWindowGridAttributeAbstractCellDataItem* activeChildItem() const;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* v);
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindow* post2dWindow() const;

	vtkIdType findCell(const QPoint& p, VTKGraphicsView* v);

	virtual SolverDefinitionGridAttribute::Position definitionPosition() const = 0;
	virtual Post2dWindowGridAttributeAbstractCellDataItem* createChild(SolverDefinitionGridAttribute* att) = 0;
	virtual vtkPointSet* filteredData() const = 0;
	virtual void getIJIndex(vtkIdType cellId, vtkIdType* i, vtkIdType* j) const = 0;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_H
