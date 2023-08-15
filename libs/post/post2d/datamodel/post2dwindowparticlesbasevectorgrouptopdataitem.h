#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <unordered_set>

class ColorMapSettingContainerI;
class Post2dWindowGridTypeDataItem;
class Post2dWindowParticlesBaseTopDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticlesBaseVectorGroupTopDataItem : public Post2dWindowDataItem
{
public:
	Post2dWindowParticlesBaseVectorGroupTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticlesBaseVectorGroupTopDataItem();

	std::unordered_set<ColorMapSettingContainerI*> activeColorMapSettingsWithVisibleLegend() const;
	void update();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void handleStandardItemChange() override;

	Post2dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post2dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* particleData() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H
