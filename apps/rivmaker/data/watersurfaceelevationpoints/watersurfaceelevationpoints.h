#ifndef WATERSURFACEELEVATIONPOINTS_H
#define WATERSURFACEELEVATIONPOINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class QVector3D;

class WaterSurfaceElevationPoints : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	WaterSurfaceElevationPoints(DataItem* parent);
	~WaterSurfaceElevationPoints();

	const std::vector<QVector3D*>& leftBankPoints() const;
	std::vector<QVector3D*>& leftBankPoints();

	const std::vector<QVector3D*>& rightBankPoints() const;
	std::vector<QVector3D*>& rightBankPoints();

	const std::vector<QVector3D*>& arbitraryPoints() const;
	std::vector<QVector3D*> arbitraryPoints();

	void clearPoints();

	void importData(QWidget* w);
	void exportData(QWidget* w);

	QStandardItem* buildPreProcessorStandardItem() const override;

private:
	std::vector<QVector3D*> m_leftBankPoints;
	std::vector<QVector3D*> m_rightBankPoints;
	std::vector<QVector3D*> m_arbitraryPoints;
};

#endif // WATERSURFACEELEVATIONPOINTS_H
