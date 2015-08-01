#include "geodatariversurvey.h"
#include "geodatariversurveycreator.h"
#include "geodatariversurveyexporter.h"
#include "geodatariversurveyimporter.h"
#include "geodatariversurveynodemapper.h"
#include "geodatariversurveyvtkexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataRiverSurveyCreator::GeoDataRiverSurveyCreator() :
	GeoDataCreator {}
{
	m_caption = tr("River Survey data");
	m_typeName = "riversurvey";

	m_importers.append(new GeoDataRiverSurveyImporter(this));
	m_exporters.append(new GeoDataRiverSurveyExporter(this));
	m_exporters.append(new GeoDataRiverSurveyVTKExporter(this));
	m_nodeMappers.append(new GeoDataRiverSurveyNodeMapper(this));
}

GeoData* GeoDataRiverSurveyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataRiverSurvey* rs = new GeoDataRiverSurvey(parent, this, condition);
	rs->setPosition(condition->position());
	rs->setDefaultMapper();
	return rs;
}

QString GeoDataRiverSurveyCreator::name(unsigned int index)
{
	return QString("riversurvey%1").arg(index);
}

QString GeoDataRiverSurveyCreator::defaultCaption(unsigned int index)
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

bool GeoDataRiverSurveyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition)
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::CellCenter) {return false;}
	if (condition->isOption()) {return false;}
	if (condition->dimensions().size() != 0) {return false;}
	return true;
}
