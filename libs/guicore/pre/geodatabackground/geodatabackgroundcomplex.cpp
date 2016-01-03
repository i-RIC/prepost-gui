#include "../base/preprocessorgeodatacomplexgroupdataiteminterface.h"
#include "../complex/gridcomplexconditionwidget.h"
#include "geodatabackgroundcomplex.h"

GeoDataBackgroundComplex::GeoDataBackgroundComplex(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoDataBackground(d, creator, att)
{}

void GeoDataBackgroundComplex::setItem(PreProcessorGeoDataComplexGroupDataItemInterface* item)
{
	m_item = item;
}

QVariant GeoDataBackgroundComplex::variantValue()
{
	int defaultVal = 1;
	for (int i = 0; i < m_item->m_widgets.count(); ++i) {
		if (m_item->m_widgets.at(i)->isDefault()) {
			defaultVal = i + 1;
		}
	}
	return defaultVal;
}
