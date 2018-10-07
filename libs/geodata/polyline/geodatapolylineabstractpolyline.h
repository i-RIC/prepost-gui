#ifndef GEODATAPOLYLINEABSTRACTPOLYLINE_H
#define GEODATAPOLYLINEABSTRACTPOLYLINE_H

#include "geodatapolylinecolorsettingdialog.h"

#include <QObject>

#include <vector>

class GeoDataPolyLine;
class PolyLineController;

class vtkScalarsToColors;

class QPointF;

class GeoDataPolyLineAbstractPolyLine : public QObject
{
	Q_OBJECT

public:
	GeoDataPolyLineAbstractPolyLine(GeoDataPolyLine* parent);
	~GeoDataPolyLineAbstractPolyLine();

	std::vector<QPointF> polyLine(const QPointF& offset = QPointF(0, 0)) const;
	void setPolyLine(const std::vector<QPointF>& p);
	std::vector<QPointF> cleanedPolyLine(const QPointF& offset = QPointF(0, 0)) const;

	bool isVertexSelectable(const QPointF& pos, double distlimit);
	bool isEdgeSelectable(const QPointF& pos, double distlimit);

	void setZDepthRange(double min, double max);

	void updateScalarValues();

	int selectedVertexId() const;
	int selectedEdgeId() const;

	void setActive(bool active);

	virtual void finishDefinition();

	void setLookupTable(vtkScalarsToColors* t);
	void setColor(const QColor& color);
	void setMapping(GeoDataPolyLineColorSettingDialog::Mapping m);

protected:
	const PolyLineController& polylineController() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatapolylineabstractpolyline_impl.h"
#endif // _DEBUG

#endif // GEODATAPOLYLINEABSTRACTPOLYLINE_H
