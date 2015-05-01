#ifndef RAWDATAPOINTMAPBREAKLINE_H
#define RAWDATAPOINTMAPBREAKLINE_H

class QVector2D;
class PreProcessorGraphicsView;
class RawDataPointmap;
#include <QObject>
#include <QVector>
#include <QVector2D>
#include <QPointF>

#include <vtkType.h>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDoubleArray.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>

class QLineF;

class RawDataPointmapBreakLine : public QObject
{
	Q_OBJECT
public:
	RawDataPointmapBreakLine(RawDataPointmap* parent);
	~RawDataPointmapBreakLine();

	bool isEdgeSelectable(const QVector2D& pos, double distlimit);

	const QVector<QPointF> polyLine() const;
	const QVector<vtkIdType>& vertexIndices() const {return m_vertexIndices;}
	vtkPolyLine* getVtkLine() {return m_vtkPolyLine;}
	void setPolyLine(const QVector<QPointF>& polyline);
	void setVertexIndices(const QVector<vtkIdType>& indices);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	int selectedEdgeId() {return m_selectedEdgeId;}

	void setActive(bool active);
	void setHidden(bool hidden);
private:
	void setupContainers();
	void setupActors();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	/// The polygon data container.
	vtkSmartPointer<vtkPolyLine> m_vtkPolyLine;
	/// The grid that has cells, those consists of the edge of m_vtkPolyLine.
	vtkSmartPointer<vtkPolyData> m_edges;
	vtkSmartPointer<vtkPolyData> m_vertices;
protected:
	RawDataPointmap* m_parent;

	QVector<vtkIdType> m_vertexIndices;
	vtkSmartPointer<vtkActor> m_edgesActor;
	vtkSmartPointer<vtkActor> m_verticesActor;
	vtkSmartPointer<vtkPolyDataMapper> m_edgesMapper;
	vtkSmartPointer<vtkPolyDataMapper> m_verticesMapper;
};

#endif // RAWDATAPOINTMAPBREAKLINE_H
