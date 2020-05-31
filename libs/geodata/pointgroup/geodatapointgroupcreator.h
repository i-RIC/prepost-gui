#ifndef GEODATAPOINTGROUPCREATOR_H
#define GEODATAPOINTGROUPCREATOR_H

#include "gd_pointgroup_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINTGROUP_EXPORT GeoDataPointGroupCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPointGroupCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOINTGROUPCREATOR_H
