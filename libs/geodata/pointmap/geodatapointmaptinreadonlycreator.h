#ifndef GEODATAPOINTMAPTINREADONLYCREATOR_H
#define GEODATAPOINTMAPTINREADONLYCREATOR_H

#include "gd_pointmap_global.h"

#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINTMAP_EXPORT GeoDataPointmapTINReadOnlyCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPointmapTINReadOnlyCreator();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	bool isReadOnly() const override;
};

#endif // GEODATAPOINTMAPTINREADONLYCREATOR_H
