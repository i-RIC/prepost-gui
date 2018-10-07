#ifndef GEODATAPOLYLINECREATOR_H
#define GEODATAPOLYLINECREATOR_H

#include "gd_polyline_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_POLYLINE_EXPORT GeoDataPolyLineCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataPolyLineCreator(const QString& typeName);

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCreatable() const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAPOLYLINECREATOR_H
