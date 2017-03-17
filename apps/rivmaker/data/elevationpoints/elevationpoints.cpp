#include "elevationpoints.h"
#include "elevationpointspreprocessorview.h"

#include <QIcon>
#include <QStandardItem>

ElevationPoints::ElevationPoints(DataItem *parent) :
	Points {parent}
{}

ElevationPoints::~ElevationPoints()
{}

QStandardItem* ElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconElevationPoints.png"), tr("Elevation Points"));
	setupStandardItem(item);
	return item;
}

DataItemView* ElevationPoints::buildPreProcessorDataItemView(Model* model)
{
	return new ElevationPointsPreprocessorView(model, this);
}
