#ifndef POINTS_H
#define POINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

class Points : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	Points(DataItem* parent);
	virtual ~Points();

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();

	void setPoints(const std::vector<QVector3D*>& points);
	void clearPoints();

	void importData(QWidget* w);
	void exportData(QWidget* w);

private:
	std::vector<QVector3D*> m_points;
};

#endif // POINTS_H
