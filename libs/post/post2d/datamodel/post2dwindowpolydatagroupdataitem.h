#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_H

#include "post2dwindowdataitem.h"

#include <guicore/misc/targeted/targeteditemi.h>
#include <postbase/polydata/polydatasetting.h>

#include <memory>
#include <unordered_map>

class ColorMapSettingContainerI;
class NamedGraphicWindowDataItem;
class v4PolyData2d;
class PolyDataSettingToolBarWidget;
class Post2dWindowAttributeBrowserController;
class Post2dWindowGridTypeDataItem;
class Post2dWindowPolyDataTopDataItem;

class Post2dWindowPolyDataGroupDataItem : public Post2dWindowDataItem, public TargetedItemI
{
	Q_OBJECT

public:
	Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* parent);
	~Post2dWindowPolyDataGroupDataItem() override;

	std::string name() const;

	std::string target() const override;
	void setTarget(const std::string& target) override;
	Post2dWindowAttributeBrowserController* attributeBrowserController() const;
	QAction* showAttributeBrowserAction() const;

	void setupActors();
	void update();
	void updateActorSetting() override;

	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	bool addToolBarButtons(QToolBar* toolBar) override;

public slots:
	void handleNamedItemChange(NamedGraphicWindowDataItem* item);

private slots:
	void showAttributeBrowser();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void doHandleResize(QResizeEvent* event, VTKGraphicsView* v) override;

	ColorMapSettingContainerI* activeSetting() const;
	Post2dWindowGridTypeDataItem* gridTypeDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowPolyDataTopDataItem* topDataItem() const;
	v4PolyData2d* polyData() const;
	void updateCheckState();

	class Impl;
	std::unique_ptr<Impl> impl;

	class AttributeBrowserController;
	class SettingEditWidget;

public:
	friend class Post2dWindowPolyDataValueDataItem;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_H
