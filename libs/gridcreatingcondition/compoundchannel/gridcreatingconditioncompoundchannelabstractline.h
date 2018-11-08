#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTLINE_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTLINE_H

class QVector2D;
class PreProcessorGraphicsView;
class GridCreatingConditionCompoundChannel;
#include <QObject>
#include <QVector2D>
#include <QPointF>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyDataMapper.h>
#include <vtkDoubleArray.h>
#include <vtkStringArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyLine.h>
#include <vtkLabeledDataMapper.h>

class QLineF;

class GridCreatingConditionCompoundChannelAbstractLine : public QObject
{
	Q_OBJECT

public:
	const static int FONTSIZE = 17; ///< Font size used for "Upstream", "DownStream" label
	GridCreatingConditionCompoundChannelAbstractLine(GridCreatingConditionCompoundChannel* parent);
	~GridCreatingConditionCompoundChannelAbstractLine();


	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);

	const QVector<QPointF> polyLine(QPointF offset = QPointF(0, 0)) const;
	vtkPolyLine* getVtkLine() const {return m_vtkPolyLine;}
	void setPolyLine(const QVector<QPointF>& polyline);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	int selectedVertexId() const {return m_selectedVertexId;}
	int selectedEdgeId() const {return m_selectedEdgeId;}

	QPointF vertex(int index) const;
	void insertVertex(int index, const QPointF& vertex);
	void setVertex(int index, const QPointF& vertex);
	void removeVertex(int index);

	void setActive(bool active);
	virtual void finishDefinition() {}
	void reverseDirection();

private:
	void setupContainers();
	void setupActors();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	/// The polygon data container.
	vtkSmartPointer<vtkPolyLine> m_vtkPolyLine;
	/// The grid that has only one cell, that is m_vtkPolyLine.
	vtkSmartPointer<vtkUnstructuredGrid> m_vtkGrid;
	/// The grid that has cells, those consists of the edge of m_vtkPolyLine.
	vtkSmartPointer<vtkUnstructuredGrid> m_edgeGrid;
	vtkSmartPointer<vtkUnstructuredGrid> m_vertexGrid;

protected:
	GridCreatingConditionCompoundChannel* m_parent;
	vtkSmartPointer<vtkStringArray> m_labelArray;

	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkActor2D> m_labelActor;

	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
	vtkSmartPointer<vtkLabeledDataMapper> m_labelMapper;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELABSTRACTLINE_H
