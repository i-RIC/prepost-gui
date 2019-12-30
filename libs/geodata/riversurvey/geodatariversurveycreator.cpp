#include "geodatariversurvey.h"
#include "geodatariversurveycreator.h"
#include "geodatariversurveyexporter.h"
#include "geodatariversurveyhonryuexporter.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveylandxmlexporter.h"
#include "geodatariversurveymlitimporter.h"
#include "geodatariversurveynodemapper.h"
#include "geodatariversurveyvtkexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QDomElement>
#include <QIcon>

GeoDataRiverSurveyCreator::GeoDataRiverSurveyCreator() :
	GeoDataCreator {"riversurvey", tr("River Survey data")}
{
	importers().push_back(new GeoDataRiverSurveyImporter(this));
	importers().push_back(new GeoDataRiverSurveyMlitImporter(this));

	exporters().push_back(new GeoDataRiverSurveyExporter(this));
	exporters().push_back(new GeoDataRiverSurveyVTKExporter(this));
	exporters().push_back(new GeoDataRiverSurveyHonryuExporter(this));
	exporters().push_back(new GeoDataRiverSurveyLandXmlExporter(this));

	nodeMappers().push_back(new GeoDataRiverSurveyNodeMapper(this));
}

GeoData* GeoDataRiverSurveyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataRiverSurvey* rs = new GeoDataRiverSurvey(parent, this, condition);
	rs->setPosition(condition->position());
	rs->setDefaultMapper();
	return rs;
}

QString GeoDataRiverSurveyCreator::name(unsigned int index) const
{
	return QString("riversurvey%1").arg(index);
}

QString GeoDataRiverSurveyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("RiverSurvey %1")).arg(index);
}

GeoData* GeoDataRiverSurveyCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
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

bool GeoDataRiverSurveyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::CellCenter) {return false;}
	if (condition->isOption()) {return false;}
	if (condition->dimensions().size() != 0) {return false;}
	return true;
}
