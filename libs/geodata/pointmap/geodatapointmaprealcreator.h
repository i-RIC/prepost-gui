#ifndef GEODATAPOINTMAPREALCREATOR_H
#define GEODATAPOINTMAPREALCREATOR_H

#include "gd_pointmap_global.h"
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/solverdef/solverdefinitiongridattributet.h>

class GD_POINTMAP_EXPORT GeoDataPointmapRealCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	/// Constructor
	GeoDataPointmapRealCreator();
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override;
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
};

#endif // GEODATAPOINTMAPREALCREATOR_H
