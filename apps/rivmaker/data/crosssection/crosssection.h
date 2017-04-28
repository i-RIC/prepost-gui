#ifndef CROSSSECTION_H
#define CROSSSECTION_H

#include "../base/dataitem.h"
#include "../base/coordinatesi.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class QPointF;

class CrossSection : public DataItem, public PreProcessorDataItemI, public CoordinatesI
{
public:
	CrossSection(DataItem* parent);
	~CrossSection();

	bool isDefined() const;

	QString name() const;

	int id() const;
	void setId(int id);

	QPointF point1() const;
	void setPoint1(const QPointF& p);

	QPointF point2() const;
	void setPoint2(const QPointF& p);

	QPointF point(int index) const;
	void setPoint(int index, const QPointF& p);

	std::vector<QPointF> coordinates() const override;
	void setCoordinates(const std::vector<QPointF>& coords) override;

	double waterElevation() const;
	void setWaterElevation(double e);

	std::vector<QVector3D*> mappedPoints() const;
	void setMappedPoints(const std::vector<QVector3D*>& points);
	void addMappedPoint(QVector3D* p);
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
