#ifndef POST2DWINDOWPARTICLESBASETOPDATAITEM_H
#define POST2DWINDOWPARTICLESBASETOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <postbase/particle/postparticlebasicsetting.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post2dWindowParticlesBaseScalarGroupDataItem;
class Post2dWindowParticlesBaseVectorGroupDataItem;
class Post2dWindowZoneDataItem;

class Post2dWindowParticlesBaseTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* parent);
	virtual ~Post2dWindowParticlesBaseTopDataItem();

	void setup();

	Post2dWindowParticlesBaseScalarGroupDataItem* scalarGroupDataItem() const;
	Post2dWindowParticlesBaseVectorGroupDataItem* vectorGroupDataItem() const;

	QColor color() const;
	void setColor(const QColor& c);

	int size() const;
	void setSize(int s);

	QAction* showAttributeBrowserAction() const;

	void updateActorSettings();
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

	virtual vtkPolyData* particleData() const = 0;
	virtual Post2dWindowZoneDataItem* zoneDataItem() const = 0;

private slots:
	void showAttributeBrowser();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setupActors();

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	Post2dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dWindowParticlesBaseVectorGroupDataItem* m_vectorGroupDataItem;

	PostParticleBasicSetting m_setting;

	QAction* m_showAttributeBrowserAction;

	class SetSettingCommand;
};

#endif // POST2DWINDOWPARTICLESBASETOPDATAITEM_H
