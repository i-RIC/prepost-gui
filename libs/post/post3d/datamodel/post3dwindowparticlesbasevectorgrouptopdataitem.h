#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <unordered_set>

class ColorMapSettingContainerI;
class Post3dWindowGridTypeDataItem;
class Post3dWindowParticlesBaseTopDataItem;
class Post3dWindowZoneDataItem;

class Post3dWindowParticlesBaseVectorGroupTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseVectorGroupTopDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowParticlesBaseVectorGroupTopDataItem();

	std::unordered_set<ColorMapSettingContainerI*> activeColorMapSettingsWithVisibleLegend() const;
	void update();
	void handleStandardItemChange() override;

	Post3dWindowParticlesBaseTopDataItem* topDataItem() const;
	Post3dWindowZoneDataItem* zoneDataItem() const;
	vtkPolyData* particleData() const;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPTOPDATAITEM_H
