#ifndef AXIS3DDATAITEM_H
#define AXIS3DDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

class GUICOREDLL_EXPORT Axis3dDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT
public:
	Axis3dDataItem(GraphicsWindowDataItem* parent);
	virtual ~Axis3dDataItem();
	void informSelection(VTKGraphicsView* v);
	void informDeselection(VTKGraphicsView* v);
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v);
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v);

protected:
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) {}
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

	void updateVisibility(bool visible);

private:
	void setupActors();
	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // AXIS3DDATAITEM_H
