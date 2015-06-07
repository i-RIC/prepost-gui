#ifndef RAWDATAPOLYGONABSTRACTPOLYGON_H
#define RAWDATAPOLYGONABSTRACTPOLYGON_H

class RawDataPolygon;
class vtkScalarsToColors;
class QVector2D;
class QPolygonF;


#include "rawdatapolygoncolorsettingdialog.h"
#include <QObject>
#include <QVector2D>
#include <QPointF>
#include <QMutex>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolygon.h>

class RawDataPolygonAbstractPolygon : public QObject
{
	Q_OBJECT

public:
	RawDataPolygonAbstractPolygon(RawDataPolygon* parent);
	~RawDataPolygonAbstractPolygon();

	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);
	bool isPolygonSelectable(const QVector2D& pos);

	const QPolygonF polygon(QPointF offset = QPointF(0, 0)) const;
	vtkPolygon* getVtkPolygon() {return m_vtkPolygon;}
	void setPolygon(const QPolygonF& p);
	void setZDepthRange(double min, double max);
	void updateShapeData();
	void updateScalarValues();
	int selectedVertexId() {return m_selectedVertexId;}
	int selectedEdgeId() {return m_selectedEdgeId;}

	mutable QMutex m_mutex;

	void setActive(bool active);
	void setSelected(bool selected);
	QPointF innerPoint(QPointF offset = QPointF(0, 0)) const;
	bool isClockwise() const;
	virtual void finishDefinition() {}
	void setLookupTable(vtkScalarsToColors* t);
	void setColor(const QColor& color);
	void setMapping(RawDataPolygonColorSettingDialog::Mapping m);

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
	vtkSmartPointer<vtkDoubleArray> m_scalarValues;

protected:
	RawDataPolygon* m_parent;

	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
};

#endif // RAWDATAPOLYGONABSTRACTPOLYGON_H
