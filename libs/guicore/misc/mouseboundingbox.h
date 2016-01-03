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

	QPoint startPoint() const;
	void setStartPoint(int x, int y);

	QPoint endPoint() const;
	void setEndPoint(int x, int y);

	void enable();
	void disable();

	void setZDepth(double z);

	bool isInsideBox(double x, double y) const;

	vtkUnstructuredGrid* vtkGrid() const;
	vtkUnstructuredGrid* vtkRegionGrid() const;
	vtkPolygon* getPolygon() const;

private:
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
