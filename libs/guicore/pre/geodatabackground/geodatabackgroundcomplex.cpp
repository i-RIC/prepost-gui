#include "../base/preprocessorgeodatacomplexgroupdataitemi.h"
#include "../complex/gridcomplexconditionwidget.h"
#include "geodatabackgroundcomplex.h"

GeoDataBackgroundComplex::GeoDataBackgroundComplex(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att) :
	GeoDataBackground(d, creator, att)
{}

void GeoDataBackgroundComplex::setItem(PreProcessorGeoDataComplexGroupDataItemI* item)
{
	m_item = item;
}

QVariant GeoDataBackgroundComplex::variantValue()
{
	int defaultVal = 1;

	auto groups = m_item->groups();
	for (int i = 0; i < groups.size(); ++i) {
		if (groups.at(i)->isDefault()) {
			defaultVal = i + 1;
		}
	}
	return defaultVal;
}
