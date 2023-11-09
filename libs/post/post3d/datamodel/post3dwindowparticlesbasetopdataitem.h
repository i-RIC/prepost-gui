#ifndef POST3DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST3DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <unordered_set>

class ColorMapSettingContainerI;
class v4Particles3d;
class Post3dWindowParticlesBaseScalarGroupDataItem;
class Post3dWindowParticlesBaseVectorGroupTopDataItem;
class Post3dWindowZoneDataItem;

class Post3dWindowParticlesBaseTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseTopDataItem(const QString& caption, Post3dWindowDataItem* parent);
	~Post3dWindowParticlesBaseTopDataItem() override;

	void setup();

	Post3dWindowParticlesBaseScalarGroupDataItem* scalarGroupDataItem() const;
	Post3dWindowParticlesBaseVectorGroupTopDataItem* vectorGroupDataItem() const;

	void update();
	void updateColorMapLegendsVisibility();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual v4Particles3d* particleData() const = 0;
	virtual Post3dWindowZoneDataItem* zoneDataItem() const = 0;

private:
	std::unordered_set<ColorMapSettingContainerI*> activeColorMapsWithVisibleLegend() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post3dWindowParticlesBaseVectorGroupTopDataItem* m_vectorGroupDataItem;
};

#endif // POST3DWINDOWPARTICLESBASETOPDATAITEM_H
