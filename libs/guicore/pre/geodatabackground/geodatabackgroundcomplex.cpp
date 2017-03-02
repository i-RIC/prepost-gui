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
	auto widgets = m_item->widgets();
	for (int i = 0; i < widgets.size(); ++i) {
		if (widgets.at(i)->isDefault()) {
			defaultVal = i + 1;
		}
	}
	return defaultVal;
}
