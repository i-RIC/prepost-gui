#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTPOLYGON_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTPOLYGON_H

class QVector2D;
class GridCreatingConditionCompoundChannel;
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

class GridCreatingConditionCompoundChannelAbstractPolygon : public QObject
{
	Q_OBJECT

public:
	GridCreatingConditionCompoundChannelAbstractPolygon(GridCreatingConditionCompoundChannel* parent);
	~GridCreatingConditionCompoundChannelAbstractPolygon();

	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);
	bool isPolygonSelectable(const QVector2D& pos);

	const QPolygonF polygon(QPointF offset = QPointF(0, 0)) const;
	vtkPolygon* getVtkPolygon() const {return m_vtkPolygon;}
	void setPolygon(const QPolygonF& p);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	int selectedVertexId() const {return m_selectedVertexId;}
	int selectedEdgeId() const {return m_selectedEdgeId;}

	QPointF vertex(int index) const;
	void insertVertex(int index, const QPointF& vertex);
	void setVertex(int index, const QPointF& vertex);
	void removeVertex(int index);

	void setActive(bool active);
	QPointF innerPoint(QPointF offset = QPointF(0, 0)) const;
	virtual void finishDefinition() {}
	int getEdgeThatIntersect(const QLineF& line) const;

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
	GridCreatingConditionCompoundChannel* m_parent;

	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTPOLYGON_H
