#include "geodatariversurvey.h"
#include "geodatariversurveycreator.h"
#include "geodatariversurveyexporter.h"
#include "geodatariversurveyhonryuexporter.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveylandxmlexporter.h"
#include "geodatariversurveymlitimporter.h"
#include "geodatariversurveynodemapper.h"
#include "geodatariversurveyodnexporter.h"
#include "geodatariversurveyodnimporter.h"
#include "geodatariversurveyvtkexporter.h"
#include "geodatariversurveywebimporter.h"

#include <QDomElement>

GeoDataRiverSurveyCreator::GeoDataRiverSurveyCreator() :
	GeoDataRiverSurveyCreator {"riversurvey"}
{
	webImporters().push_back(new GeoDataRiverSurveyWebImporter(this));
}

GeoData* GeoDataRiverSurveyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto rs = new GeoDataRiverSurvey(parent, this, condition);
	rs->setMapper(new GeoDataRiverSurveyNodeMapper(this));
	return rs;
}

QString GeoDataRiverSurveyCreator::name(unsigned int index) const
{
	return QString("riversurvey%1").arg(index);
}

QString GeoDataRiverSurveyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Cross-Section Data%1")).arg(index);
}

bool GeoDataRiverSurveyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() != SolverDefinitionGridAttribute::Position::Node) {return false;}
	if (condition->isOption()) {return false;}
	if (condition->dimensions().size() != 0) {return false;}
	if (condition->isReferenceInformation()) {return false;}

	return true;
}

bool GeoDataRiverSurveyCreator::isCreatable() const
{
	return true;
}

GeoDataRiverSurveyCreator::GeoDataRiverSurveyCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Cross-Section Data")}
{
	importers().push_back(new GeoDataRiverSurveyImporter(this));
	importers().push_back(new GeoDataRiverSurveyMlitImporter(this));
	importers().push_back(new GeoDataRiverSurveyOdnImporter(this));

	exporters().push_back(new GeoDataRiverSurveyExporter(this));
	exporters().push_back(new GeoDataRiverSurveyHonryuExporter(this));
	exporters().push_back(new GeoDataRiverSurveyLandXmlExporter(this));
	exporters().push_back(new GeoDataRiverSurveyOdnExporter(this));
}
