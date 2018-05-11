#ifndef ATTRIBUTEBROWSERTARGETDATAITEM_H
#define ATTRIBUTEBROWSERTARGETDATAITEM_H

#include "../guicore_global.h"
#include "graphicswindowdataitem.h"

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

class QVector2D;
class QPolygonF;

class GUICOREDLL_EXPORT AttributeBrowserTargetDataItem : public GraphicsWindowDataItem
{
	Q_OBJECT

public:
	AttributeBrowserTargetDataItem(GraphicsWindowDataItem* parent);
	~AttributeBrowserTargetDataItem();

	void updateZDepthRangeItemCount() override;
	void assignActorZValues(const ZDepthRange& range) override;

	void setPoint(const QVector2D& v);
	void setPolygon(const QPolygonF& p);
	void clear();

private:
	void setupContainers();
	void setupActors();

	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	vtkSmartPointer<vtkPolygon> m_polygon;

	vtkSmartPointer<vtkPolyData> m_vertex;
	vtkSmartPointer<vtkPolyData> m_polygonEdge;
	vtkSmartPointer<vtkPolyData> m_polygonPaint;

	vtkSmartPointer<vtkPolyDataMapper> m_paintMapper;
	vtkSmartPointer<vtkPolyDataMapper> m_edgeMapper;
	vtkSmartPointer<vtkPolyDataMapper> m_vertexMapper;

	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;

	vtkSmartPointer<vtkAxesActor> m_actor;
	vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // ATTRIBUTEBROWSERTARGETDATAITEM_H
