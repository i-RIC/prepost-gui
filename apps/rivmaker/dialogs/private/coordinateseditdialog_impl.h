#ifndef COORDINATESEDITDIALOG_IMPL_H
#define COORDINATESEDITDIALOG_IMPL_H

#include <vector>
#include <QPointF>
#include <QStandardItemModel>

#include "../coordinateseditdialog.h"

class CoordinatesEditDialog::Impl
{
public:
	Impl(QWidget* w);

	QStandardItemModel m_model;

	std::vector<QPointF> m_oldCoordinates;
	QPointF m_offset;
	CoordinatesI* m_target;
};

#endif // COORDINATESEDITDIALOG_IMPL_H
