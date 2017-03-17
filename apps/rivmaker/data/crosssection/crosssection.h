#ifndef CROSSSECTION_H
#define CROSSSECTION_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <QPointF>

class CrossSection : public DataItem, public PreProcessorDataItemI
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

	QPointF crossSectionPoint() const;
	void setCrossSectionPoint(const QPointF& p);

	void getNearestPoint(double x, double y, QPointF* nearestPoint, double* distance, double* pos) const;

	void reverseDirection();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemController* buildPreProcessorDataItemController(Model* model) override;
	DataItemView* buildPreProcessorDataItemView(Model* model) override;

private:
	int m_id;
	bool m_isDefined;

	QPointF m_point1;
	QPointF m_point2;

	QPointF m_crossSectionPoint;
};

#endif // CROSSSECTION_H
