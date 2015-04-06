#ifndef POST2DBIRDEYEWINDOWAXESDATAITEM_H
#define POST2DBIRDEYEWINDOWAXESDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

class Post2dBirdEyeWindowAxesDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
public:
	Post2dBirdEyeWindowAxesDataItem(GraphicsWindowDataItem* parent);
	~Post2dBirdEyeWindowAxesDataItem();
	void informSelection(VTKGraphicsView * /*v*/);
	void informDeselection(VTKGraphicsView* /*v*/);
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/);
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/){}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/){}

	void updateVisibility(bool visible);
private:
	void setupActors();
	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // POST2DBIRDEYEWINDOWAXESDATAITEM_H
