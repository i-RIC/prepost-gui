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
	void informSelection(VTKGraphicsView* /*v*/) override;
	void informDeselection(VTKGraphicsView* /*v*/) override;
	void mouseMoveEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mousePressEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, VTKGraphicsView* /*v*/) override;
protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override {}

	void updateVisibility(bool visible) override;
private:
	void setupActors();
	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // POST2DBIRDEYEWINDOWAXESDATAITEM_H
