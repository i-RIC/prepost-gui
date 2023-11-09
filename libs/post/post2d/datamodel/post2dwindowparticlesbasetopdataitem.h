#ifndef POST2DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST2DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <memory>
#include <unordered_set>

class ColorMapSettingContainerI;
class v4Particles2d;
class Post2dWindowAttributeBrowserController;
class Post2dWindowCalculationResultDataItem;
class Post2dWindowParticlesBaseScalarGroupDataItem;
class Post2dWindowParticlesBaseVectorGroupTopDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticlesBaseTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* parent);
	~Post2dWindowParticlesBaseTopDataItem() override;

	void setup();

	Post2dWindowZoneDataItem* zoneDataItem() const;
	Post2dWindowParticlesBaseScalarGroupDataItem* scalarGroupDataItem() const;
	Post2dWindowParticlesBaseVectorGroupTopDataItem* vectorGroupDataItem() const;
	Post2dWindowAttributeBrowserController* attributeBrowserController() const;

	QAction* showAttributeBrowserAction() const;

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void update();
	void updateColorMapLegendsVisibility();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual v4Particles2d* particleData() const = 0;
	virtual Post2dWindowCalculationResultDataItem* resultDataItem() const = 0;

private slots:
	void showAttributeBrowser();

private:
	std::unordered_set<ColorMapSettingContainerI*> activeColorMapsWithVisibleLegend() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	class Impl;
	std::unique_ptr<Impl> impl;

	class AttributeBrowserController;
;
};

#endif // POST2DWINDOWPARTICLESBASETOPDATAITEM_H
