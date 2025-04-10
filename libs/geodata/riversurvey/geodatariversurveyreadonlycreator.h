#ifndef GEODATARIVERSURVEYREADONLYCREATOR_H
#define GEODATARIVERSURVEYREADONLYCREATOR_H

#include "gd_riversurvey_global.h"
#include <guicore/pre/geodata/geodatacreator.h>

class GeoDataRiverSurveyReadOnlyCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	GeoDataRiverSurveyReadOnlyCreator();

	QString name(unsigned int index) const override;
	QString defaultCaption(unsigned int index) const override;

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	GeoData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;

	bool isCreatable() const override;
	bool isReadOnly() const override;
};

#endif // GEODATARIVERSURVEYREADONLYCREATOR_H
