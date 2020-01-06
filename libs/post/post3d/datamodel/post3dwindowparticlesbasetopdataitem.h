#ifndef POST3DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST3DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <postbase/particle/postparticlebasicsetting.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

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

	QColor color() const;
	void setColor(const QColor& c);

	int size() const;
	void setSize(int s);

	void updateActorSettings();
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual vtkPolyData* particleData() const = 0;
	virtual Post3dWindowZoneDataItem* zoneDataItem() const = 0;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;
	void setupActors();

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post3dWindowParticlesBaseVectorGroupDataItem* m_vectorGroupDataItem;

	PostParticleBasicSetting m_setting;

	class SetSettingCommand;
};

#endif // POST3DWINDOWPARTICLESBASETOPDATAITEM_H
