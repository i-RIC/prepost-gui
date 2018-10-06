#ifndef GEODATAPOLYGONABSTRACTPOLYGON_H
#define GEODATAPOLYGONABSTRACTPOLYGON_H

#include "geodatapolygoncolorsettingdialog.h"

#include <guibase/polygon/polygoncontroller.h>

#include <QObject>

class GeoDataPolygon;

class vtkDoubleArray;
class vtkPolygon;
class vtkScalarsToColors;

class QPolygonF;
class QPointF;

class GeoDataPolygonAbstractPolygon : public QObject
{
	Q_OBJECT

public:
	GeoDataPolygonAbstractPolygon(GeoDataPolygon* parent);
	~GeoDataPolygonAbstractPolygon();

	QPolygonF polygon(const QPointF& offset = QPointF(0, 0)) const;
	void setPolygon(const QPolygonF& p);
	QPolygonF cleanedPolygon(const QPointF& offset = QPointF(0, 0)) const;

	bool isVertexSelectable(const QPointF& pos, double distlimit);
	bool isEdgeSelectable(const QPointF& pos, double distlimit);
	bool isPolygonSelectable(const QPointF& pos);

	vtkPolygon* getVtkPolygon() const;
	void applyVtkPolygonShape();

	void setZDepthRange(double min, double max);

	void updateScalarValues();

	int selectedVertexId() const;
	int selectedEdgeId() const;

	void setActive(bool active);
	void setSelected(bool selected);

	virtual void finishDefinition();

	void setLookupTable(vtkScalarsToColors* t);
	void setColor(const QColor& color);
	void setMapping(GeoDataPolygonColorSettingDialog::Mapping m);

protected:
	const PolygonController& polygonController() const;

private:
	void setupScalarValues();

	int m_selectedVertexId;
	int m_selectedEdgeId;

	GeoDataPolygon* m_parent;
	PolygonController m_polygonController;
	vtkDoubleArray* m_scalarValues;
};

#endif // GEODATAPOLYGONABSTRACTPOLYGON_H
