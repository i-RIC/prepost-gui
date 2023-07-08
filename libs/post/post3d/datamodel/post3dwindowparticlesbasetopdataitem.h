#ifndef POST3DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST3DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <unordered_set>

class ColorMapSettingContainerI;
class Post3dWindowParticlesBaseScalarGroupDataItem;
class Post3dWindowParticlesBaseVectorGroupDataItem;
class Post3dWindowZoneDataItem;

class Post3dWindowParticlesBaseTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseTopDataItem(const QString& caption, Post3dWindowDataItem* parent);
	~Post3dWindowParticlesBaseTopDataItem();

	void setup();

	Post3dWindowParticlesBaseScalarGroupDataItem* scalarGroupDataItem() const;
	Post3dWindowParticlesBaseVectorGroupDataItem* vectorGroupDataItem() const;

	void update();
	void updateColorMaps();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual vtkPolyData* particleData() const = 0;
	virtual Post3dWindowZoneDataItem* zoneDataItem() const = 0;

private:
	std::unordered_set<ColorMapSettingContainerI*> activeColorMaps() const;

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post3dWindowParticlesBaseVectorGroupDataItem* m_vectorGroupDataItem;
};

#endif // POST3DWINDOWPARTICLESBASETOPDATAITEM_H
