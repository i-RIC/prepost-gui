#ifndef GEODATABACKGROUNDCOMPLEX_H
#define GEODATABACKGROUNDCOMPLEX_H

#include "../../guicore_global.h"
#include "geodatabackground.h"

class PreProcessorGeoDataComplexGroupDataItemInterface;

class GUICOREDLL_EXPORT GeoDataBackgroundComplex : public GeoDataBackground
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
