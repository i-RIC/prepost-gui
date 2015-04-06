#ifndef MOUSEBOUNDINGBOX_H
#define MOUSEBOUNDINGBOX_H

#include "../guicore_global.h"
#include <QObject>
#include <QPoint>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

class VTK2DGraphicsView;

class GUICOREDLL_EXPORT MouseBoundingBox : public QObject
{
public:
	MouseBoundingBox(VTK2DGraphicsView* view, QObject* parent);
	void setStartPoint(int x, int y);
	void setEndPoint(int x, int y);
	const QPoint& startPoint() const {return m_startPoint;}
	const QPoint& endPoint() const {return m_endPoint;}
	void enable();
	void disable();
	void setZDepth(double z);
	bool isInsideBox(double x, double y);
	vtkUnstructuredGrid* vtkGrid(){return m_lineGrid;}
	vtkUnstructuredGrid* vtkRegionGrid(){return m_regionGrid;}
	vtkPolygon* getPolygon(){return m_polygon;}
protected:
	vtkSmartPointer<vtkPolygon> m_polygon;
	vtkSmartPointer<vtkUnstructuredGrid> m_lineGrid;
	vtkSmartPointer<vtkUnstructuredGrid> m_regionGrid;
	vtkSmartPointer<vtkActor> m_lineActor;
	vtkSmartPointer<vtkDataSetMapper> m_lineMapper;
	vtkSmartPointer<vtkUnstructuredGrid> m_paintGrid;
	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;

	VTK2DGraphicsView* m_graphicsView;
	QPoint m_startPoint;
	QPoint m_endPoint;
};

#endif // MOUSEBOUNDINGBOX_H
