#ifndef GEODATARIVERSURVEYCREATOR_H
#define GEODATARIVERSURVEYCREATOR_H

#include "gd_riversurvey_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataRiverSurveyCreator();

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	bool isCreatable() const override;

	GeoDataRiverSurveyCreator(const QString& typeName);
};

#endif // GEODATARIVERSURVEYCREATOR_H
