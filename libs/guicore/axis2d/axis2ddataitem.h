#ifndef AXIS2DDATAITEM_H
#define AXIS2DDATAITEM_H

#include "../guicore_global.h"
#include "../datamodel/graphicswindowdataitem.h"

#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>

class GUICOREDLL_EXPORT Axis2dDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	Axis2dDataItem(GraphicsWindowDataItem* parent);
	virtual ~Axis2dDataItem();
	void informSelection(VTKGraphicsView* v) override;
	void informDeselection(VTKGraphicsView* v) override;
	void mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mousePressEvent(QMouseEvent* event, VTKGraphicsView* v) override;
	void mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v) override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void updateVisibility(bool visible) override;

private:
	void setupActors();
	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // AXIS2DDATAITEM_H
