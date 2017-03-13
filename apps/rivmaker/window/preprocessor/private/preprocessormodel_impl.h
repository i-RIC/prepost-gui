#ifndef PREPROCESSORMODEL_IMPL_H
#define PREPROCESSORMODEL_IMPL_H

#include "../preprocessormodel.h"

#include <QStandardItem>
#include <QStandardItemModel>

class PreProcessorModel::Impl
{
public:
	Impl();
	~Impl();

	void setupStandatdItemModel();

	Project* m_project;
	PreProcessorView* m_view;

	QStandardItemModel m_standardItemModel;

	QStandardItem m_elevationPointsItem;
	QStandardItem m_waterSurfaceElevationPointsItem;
	QStandardItem m_crossSectionsItem;
	QStandardItem m_baseLineItem;
	QStandardItem m_riverSurveyDataItem;
};

#endif // PREPROCESSORMODEL_IMPL_H
