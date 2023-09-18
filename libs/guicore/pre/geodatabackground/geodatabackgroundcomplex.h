#ifndef GEODATABACKGROUNDCOMPLEX_H
#define GEODATABACKGROUNDCOMPLEX_H

#include "../../guicore_global.h"
#include "geodatabackground.h"

class PreProcessorGeoDataComplexGroupDataItemI;

class GUICOREDLL_EXPORT GeoDataBackgroundComplex : public GeoDataBackground
{
	Q_OBJECT

public:
	GeoDataBackgroundComplex(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);

	void setItem(PreProcessorGeoDataComplexGroupDataItemI* item);

	QVariant variantValue() override;

private:
	PreProcessorGeoDataComplexGroupDataItemI* m_item;
};


#endif // GEODATABACKGROUNDCOMPLEX_H
