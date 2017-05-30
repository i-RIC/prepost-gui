#ifndef CROSSSECTION_H
#define CROSSSECTION_H

#include "../base/dataitem.h"
#include "../base/coordinatesi.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class QPointF;
class QVector2D;

class CrossSection : public DataItem, public PreProcessorDataItemI, public CoordinatesI
{
public:
	CrossSection(DataItem* parent);
	~CrossSection();

	bool isDefined() const;

	QString name() const;
	void setName(const QString& name);

	QPointF point1() const;
	void setPoint1(const QPointF& p);

	QPointF point2() const;
	void setPoint2(const QPointF& p);

	QPointF point(int index) const;
	void setPoint(int index, const QPointF& p);

	std::vector<QPointF> coordinates() const override;
	void setCoordinates(const std::vector<QPointF>& coords) override;

	bool waterElevationIsSet() const;
	void setWaterElevationIsSet(bool isSet);

	double waterElevation() const;
	void setWaterElevation(double e);

	std::vector<QVector2D> mappedPoints() const;
	void setMappedPoints(const std::vector<QVector2D>& points);
	void addMappedPoint(const QVector2D& p);
	void clearMappedPoints();

	void getNearestPoint(double x, double y, QPointF* nearestPoint, double* distance) const;
	void getMappedPoint(double x, double y, QPointF* mappedPoint, double* position) const;

	void reverseDirection();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemController* buildPreProcessorDataItemController(Model* model) override;
	DataItemView* buildPreProcessorDataItemView(Model* model) override;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/crosssection_impl.h"
#endif // _DEBUG

#endif // CROSSSECTION_H
