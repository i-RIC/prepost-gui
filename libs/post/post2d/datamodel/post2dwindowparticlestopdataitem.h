#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <postbase/particle/postparticlebasicsetting.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post2dWindowParticlesScalarGroupDataItem;
class Post2dWindowParticlesVectorGroupDataItem;

class Post2dWindowParticlesTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticlesTopDataItem();

	Post2dWindowParticlesScalarGroupDataItem* scalarGroupDataItem() const;
	Post2dWindowParticlesVectorGroupDataItem* vectorGroupDataItem() const;

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

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void setupActors();

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	Post2dWindowParticlesScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dWindowParticlesVectorGroupDataItem* m_vectorGroupDataItem;

	PostParticleBasicSetting m_setting;

	class SetSettingCommand;
};

#ifdef _DEBUG
	#include "private/post2dwindowparticlestopdataitem_setsettingcommand.h"
#endif // _DEBUG

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_H
