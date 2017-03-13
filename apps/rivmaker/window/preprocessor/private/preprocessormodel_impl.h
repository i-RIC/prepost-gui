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
};

#endif // PREPROCESSORMODEL_IMPL_H
