#ifndef POST3DWINDOWPARTICLESTOPDATAITEM_H
#define POST3DWINDOWPARTICLESTOPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post3dWindowParticlesTopSetProperty;

class Post3dWindowParticlesTopDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowParticlesTopDataItem();
	void updateActorSettings();
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActorZValues(const ZDepthRange& range) override;
	void update();
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void innerUpdateZScale(double scale) override;

private:
	void setupActors();

protected:
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	QColor m_color;
	int m_size;

public:
	friend class Post3dWindowParticlesTopSetProperty;
};

#endif // POST3DWINDOWPARTICLESTOPDATAITEM_H
