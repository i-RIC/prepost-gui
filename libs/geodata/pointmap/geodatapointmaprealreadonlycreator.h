#ifndef GEODATAPOINTMAPREALREADONLYCREATOR_H
#define GEODATAPOINTMAPREALREADONLYCREATOR_H

#include "gd_pointmap_global.h"

#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINTMAP_EXPORT GeoDataPointmapRealReadOnlyCreator : public GeoDataCreator
{
	Q_OBJECT
public:
	GeoDataPointmapRealReadOnlyCreator();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	bool isReadOnly() const override;
};

#endif // GEODATAPOINTMAPREALREADONLYCREATOR_H
