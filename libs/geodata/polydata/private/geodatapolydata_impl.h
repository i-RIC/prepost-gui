#ifndef GEODATAPOLYDATA_IMPL_H
#define GEODATAPOLYDATA_IMPL_H

#include "../geodatapolydata.h"

#include <QVariant>

#include <vector>

class GeoDataPolyData::Impl
{
public:
	Impl(GeoDataPolyData* parent);

	std::vector<QVariant> m_variantValues;

	QAction* m_editNameAction;
	QAction* m_editValueAction;
	QAction* m_editNameAndValueAction;
	QAction* m_editColorSettingAction;
};

#endif // GEODATAPOLYDATA_IMPL_H
