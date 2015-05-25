#ifndef POST3DWINDOWSURFACEGROUPDATAITEM_H
#define POST3DWINDOWSURFACEGROUPDATAITEM_H

#include "../post3dwindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>
#include <vtkActor.h>
#include <vtkMapper.h>

#include <QColor>

class Post3dWindowSurfaceGroupDataItem : public Post3dWindowDataItem
{
	Q_OBJECT
public:
	/// Constructor
	Post3dWindowSurfaceGroupDataItem(Post3dWindowDataItem* parent);
	void updateActorSettings();
	void setupActors();
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}
private:
	vtkSmartPointer<vtkExtractGrid> m_extractGrid;
	vtkSmartPointer<vtkActor> m_actor;
	vtkSmartPointer<vtkMapper> m_mapper;

	unsigned int m_iMin;
	unsigned int m_iMax;
	unsigned int m_jMin;
	unsigned int m_jMax;
	unsigned int m_kMin;
	unsigned int m_kMax;

	QColor m_color;
};

#endif // POST3DWINDOWSURFACEGROUPDATAITEM_H
