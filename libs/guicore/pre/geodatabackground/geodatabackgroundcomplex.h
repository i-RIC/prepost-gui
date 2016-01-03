#ifndef GEODATABACKGROUNDCOMPLEX_H
#define GEODATABACKGROUNDCOMPLEX_H

#include "geodatabackground.h"

class PreProcessorGeoDataComplexGroupDataItemInterface;

class GeoDataBackgroundComplex : public GeoDataBackground
{
	Q_OBJECT

public:
	GeoDataBackgroundComplex(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);

	void setItem(PreProcessorGeoDataComplexGroupDataItemInterface* item);

	QVariant variantValue() override;

private:
	PreProcessorGeoDataComplexGroupDataItemInterface* m_item;
};


#endif // GEODATABACKGROUNDCOMPLEX_H
