#include "geodatariversurvey.h"
#include "geodatariversurveyexporter.h"
#include "geodatariversurveyhonryuexporter.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveylandxmlexporter.h"
#include "geodatariversurveymlitimporter.h"
#include "geodatariversurveynodemapper.h"
#include "geodatariversurveyreadonlycreator.h"
#include "geodatariversurveyvtkexporter.h"
#include "geodatariversurveywebimporter.h"

#include <QAction>
#include <QDomElement>
#include <QIcon>

GeoDataRiverSurveyReadOnlyCreator::GeoDataRiverSurveyReadOnlyCreator() :
	GeoDataCreator {"riversurveyReadOnly", tr("Cross-Section Data")}
{
	importers().push_back(new GeoDataRiverSurveyImporter(this));
	importers().push_back(new GeoDataRiverSurveyMlitImporter(this));

	exporters().push_back(new GeoDataRiverSurveyExporter(this));
	exporters().push_back(new GeoDataRiverSurveyHonryuExporter(this));
	exporters().push_back(new GeoDataRiverSurveyLandXmlExporter(this));
}

GeoData* GeoDataRiverSurveyReadOnlyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataRiverSurvey* rs = new GeoDataRiverSurvey(parent, this, condition);
	rs->setMapper(new GeoDataRiverSurveyNodeMapper(this));
	return rs;
}

QString GeoDataRiverSurveyReadOnlyCreator::name(unsigned int index) const
{
	return QString("riversurvey%1").arg(index);
}

QString GeoDataRiverSurveyReadOnlyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Cross-Section Data%1")).arg(index);
}

GeoData* GeoDataRiverSurveyReadOnlyCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoData* geodata = GeoDataCreator::restore(node, parent, condition);
	if (geodata != nullptr) {return geodata;}
	QDomElement elem = node.toElement();
	QString name = elem.attribute("name");
	if (name.contains("riversurvey")) {
		return create(parent, condition);
	}
	return nullptr;
}

bool GeoDataRiverSurveyReadOnlyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() != SolverDefinitionGridAttribute::Position::Node) {return false;}
	if (condition->isOption()) {return false;}
	if (condition->dimensions().size() != 0) {return false;}
	if (condition->isReferenceInformation()) {return false;}

	return true;
}

bool GeoDataRiverSurveyReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataRiverSurveyReadOnlyCreator::isReadOnly() const
{
	return true;
}
