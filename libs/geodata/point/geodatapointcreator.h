#ifndef GEODATAPOINTCREATOR_H
#define GEODATAPOINTCREATOR_H

#include "gd_point_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_POINT_EXPORT GeoDataPointCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPointCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOINTCREATOR_H
