#ifndef GRIDCREATINGCONDITIONTRIANGLEABSTRACTLINE_H
#define GRIDCREATINGCONDITIONTRIANGLEABSTRACTLINE_H

class QVector2D;
class PreProcessorGraphicsView;
class GridCreatingConditionTriangle;
#include <QObject>
#include <QVector2D>
#include <QPointF>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyLine.h>

class QLineF;

class GridCreatingConditionTriangleAbstractLine : public QObject
{
	Q_OBJECT
public:
	GridCreatingConditionTriangleAbstractLine(GridCreatingConditionTriangle* parent);
	~GridCreatingConditionTriangleAbstractLine();


	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);

	const QVector<QPointF> polyLine(QPointF offset = QPointF(0, 0)) const;
	vtkPolyLine* getVtkLine(){return m_vtkPolyLine;}
	void setPolyLine(const QVector<QPointF>& polyline);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	int selectedVertexId(){return m_selectedVertexId;}
	int selectedEdgeId(){return m_selectedEdgeId;}

	void setActive(bool active);
	virtual void finishDefinition(){}
private:
	void setupContainers();
	void setupActors();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	/// The polygon data container.
	vtkSmartPointer<vtkPolyLine> m_vtkPolyLine;
	/// The grid that has only one cell, that is m_vtkPolyLine.
	/// The grid that has cells, those consists of the edge of m_vtkPolyLine.
	vtkSmartPointer<vtkUnstructuredGrid> m_edgeGrid;
	vtkSmartPointer<vtkUnstructuredGrid> m_vertexGrid;
protected:
	GridCreatingConditionTriangle* m_parent;

	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEABSTRACTLINE_H
