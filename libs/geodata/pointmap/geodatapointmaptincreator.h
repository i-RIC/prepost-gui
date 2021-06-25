#ifndef GEODATAPOINTMAPTINCREATOR_H
#define GEODATAPOINTMAPTINCREATOR_H

#include "gd_pointmap_global.h"

#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINTMAP_EXPORT GeoDataPointmapTINCreator : public GeoDataCreator
{
public:
	GeoDataPointmapTINCreator();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOINTMAPTINCREATOR_H
