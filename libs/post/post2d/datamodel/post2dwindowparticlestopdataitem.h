#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_H

#include "../post2dwindowdataitem.h"
#include <postbase/postparticlebasicpropertydialog.h>

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post2dWindowParticlesScalarGroupDataItem;
class Post2dWindowParticlesVectorGroupDataItem;
class Post2dWindowParticlesTopSetProperty;

class Post2dWindowParticlesTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT

public:
	Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticlesTopDataItem();

	int size() const;
	void setSize(int size);

	QColor color() const;
	void setColor(const QColor& color);

	Post2dWindowParticlesScalarGroupDataItem* scalarGroupDataItem() const {m_scalarGroupDataItem;}
	Post2dWindowParticlesVectorGroupDataItem* vectorGroupDataItem() const {m_vectorGroupDataItem;}

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

	PostParticleBasicPropertyDialog::Setting m_setting;

	Post2dWindowParticlesScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dWindowParticlesVectorGroupDataItem* m_vectorGroupDataItem;

	class SetSettingCommand;
};

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_H
