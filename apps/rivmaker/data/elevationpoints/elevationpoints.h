#ifndef ELEVATIONPOINTS_H
#define ELEVATIONPOINTS_H

#include "../base/dataitem.h"
#include "../../window/preprocessor/preprocessordataitemi.h"

#include <vector>

class QVector3D;

class ElevationPoints : public DataItem, public PreProcessorDataItemI
{
	Q_OBJECT

public:
	ElevationPoints(DataItem* parent);
	~ElevationPoints();

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();
	void clearPoints();

	void importData(QWidget* w);
	void exportData(QWidget* w);

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

private:
	std::vector<QVector3D*> m_points;
};

#endif // ELEVATIONPOINTS_H
