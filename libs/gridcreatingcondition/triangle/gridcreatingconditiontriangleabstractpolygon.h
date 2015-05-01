#ifndef GRIDCREATINGCONDITIONTRIANGLEABSTRACTPOLYGON_H
#define GRIDCREATINGCONDITIONTRIANGLEABSTRACTPOLYGON_H

class QVector2D;
class GridCreatingConditionTriangle;
#include <QObject>
#include <QVector2D>
#include <QPointF>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolygon.h>
class QPolygonF;

class GridCreatingConditionTriangleAbstractPolygon : public QObject
{
	Q_OBJECT
public:
	GridCreatingConditionTriangleAbstractPolygon(GridCreatingConditionTriangle* parent);
	~GridCreatingConditionTriangleAbstractPolygon();


	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);
	bool isPolygonSelectable(const QVector2D& pos);

	const QPolygonF polygon(QPointF offset = QPointF(0, 0)) const;
	vtkPolygon* getVtkPolygon() {return m_vtkPolygon;}
	void setPolygon(const QPolygonF& p);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	int selectedVertexId() {return m_selectedVertexId;}
	int selectedEdgeId() {return m_selectedEdgeId;}

	void setActive(bool active);
	QPointF innerPoint(QPointF offset = QPointF(0, 0)) const;
	virtual void finishDefinition() {}
private:
	void setupContainers();
	void setupActors();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	/// The polygon data container.
	vtkSmartPointer<vtkPolygon> m_vtkPolygon;
	/// The grid that has only one cell, that is m_vtkPolygon.
	vtkSmartPointer<vtkUnstructuredGrid> m_vtkGrid;
	/// The grid that has cells, those consists of the edge of m_vtkPolygon.
	vtkSmartPointer<vtkUnstructuredGrid> m_edgeGrid;
	vtkSmartPointer<vtkUnstructuredGrid> m_vertexGrid;
protected:
	GridCreatingConditionTriangle* m_parent;

	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEABSTRACTPOLYGON_H
