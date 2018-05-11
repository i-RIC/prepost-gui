#ifndef GEODATAPOINTMAPREALCREATOR_H
#define GEODATAPOINTMAPREALCREATOR_H

#include "gd_pointmap_global.h"

#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINTMAP_EXPORT GeoDataPointmapRealCreator : public GeoDataCreator
{
	Q_OBJECT
public:
	GeoDataPointmapRealCreator();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOINTMAPREALCREATOR_H
