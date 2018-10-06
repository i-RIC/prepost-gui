#ifndef GEODATAPOLYGONABSTRACTPOLYGON_H
#define GEODATAPOLYGONABSTRACTPOLYGON_H

#include "geodatapolygoncolorsettingdialog.h"

#include <vtkDataSetMapper.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

#include <QObject>

class GeoDataPolygon;

class vtkScalarsToColors;

class QPolygonF;
class QVector2D;

class GeoDataPolygonAbstractPolygon : public QObject
{
	Q_OBJECT

public:
	GeoDataPolygonAbstractPolygon(GeoDataPolygon* parent);
	~GeoDataPolygonAbstractPolygon();

	bool isVertexSelectable(const QVector2D& pos, double distlimit);
	bool isEdgeSelectable(const QVector2D& pos, double distlimit);
	bool isPolygonSelectable(const QVector2D& pos);

	const QPolygonF polygon(QPointF offset = QPointF(0, 0), bool noClean = false) const;

	vtkPolygon* getVtkPolygon() const;
	void setPolygon(const QPolygonF& p);

	void setZDepthRange(double min, double max);

	void updateShapeData();
	void updateScalarValues();

	int selectedVertexId() const;
	int selectedEdgeId() const;

	mutable QMutex m_mutex;

	void setActive(bool active);
	void setSelected(bool selected);

	QPointF innerPoint(QPointF offset = QPointF(0, 0)) const;

	virtual void finishDefinition();

	void setLookupTable(vtkScalarsToColors* t);
	void setColor(const QColor& color);
	void setMapping(GeoDataPolygonColorSettingDialog::Mapping m);

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
	GeoDataPolygon* m_parent;

	vtkSmartPointer<vtkActor> m_paintActor;
	vtkSmartPointer<vtkActor> m_edgeActor;
	vtkSmartPointer<vtkActor> m_vertexActor;
	vtkSmartPointer<vtkDataSetMapper> m_paintMapper;
	vtkSmartPointer<vtkDataSetMapper> m_edgeMapper;
	vtkSmartPointer<vtkDataSetMapper> m_vertexMapper;
};

#endif // GEODATAPOLYGONABSTRACTPOLYGON_H
