#ifndef POST3DWINDOWPARTICLESTOPDATAITEM_H
#define POST3DWINDOWPARTICLESTOPDATAITEM_H

#include "../post3dwindowdataitem.h"
#include <postbase/postparticlebasicpropertydialog.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post3dWindowParticlesScalarGroupDataItem;
class Post3dWindowParticlesVectorGroupDataItem;

class Post3dWindowParticlesTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	/// Constructor
	Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowParticlesTopDataItem();

	Post3dWindowParticlesScalarGroupDataItem* scalarGroupDataItem() const;
	Post3dWindowParticlesVectorGroupDataItem* vectorGroupDataItem() const;

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
	void innerUpdateZScale(double scale) override;
	void setupActors();

	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	Post3dWindowParticlesScalarGroupDataItem* m_scalarGroupDataItem;
	Post3dWindowParticlesVectorGroupDataItem* m_vectorGroupDataItem;

	PostParticleBasicPropertyDialog::Setting m_setting;

	class SetSettingCommand;
};

#endif // POST3DWINDOWPARTICLESTOPDATAITEM_H
