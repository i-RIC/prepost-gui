#ifndef POST2DWINDOWPARTICLESTOPDATAITEM_H
#define POST2DWINDOWPARTICLESTOPDATAITEM_H

#include "../post2dwindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

class Post2dWindowParticlesTopSetProperty;

class Post2dWindowParticlesTopDataItem : public Post2dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowParticlesTopDataItem();
	void updateActorSettings();
	void updateZDepthRangeItemCount(ZDepthRange& range);
	void assignActionZValues(const ZDepthRange& range);
	void update();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	void setupActors();

protected:
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkPolyDataMapper> m_mapper;

	QColor m_color;
	int m_size;

public:
	friend class Post2dWindowParticlesTopSetProperty;
};

#endif // POST2DWINDOWPARTICLESTOPDATAITEM_H
