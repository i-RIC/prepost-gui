#include "rawdatariversurvey.h"
#include "rawdatariversurveycreator.h"
#include "rawdatariversurveyexporter.h"
#include "rawdatariversurveyimporter.h"
#include "rawdatariversurveynodemapper.h"
#include "rawdatariversurveyvtkexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

RawDataRiverSurveyCreator::RawDataRiverSurveyCreator()
	: RawDataCreator()
{
	m_caption = tr("River Survey data");
	m_typeName = "riversurvey";

	m_importers.append(new RawDataRiverSurveyImporter(this));
	m_exporters.append(new RawDataRiverSurveyExporter(this));
	m_exporters.append(new RawDataRiverSurveyVTKExporter(this));
	m_nodeMappers.append(new RawDataRiverSurveyNodeMapper(this));
}

RawData* RawDataRiverSurveyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	RawDataRiverSurvey* rs = new RawDataRiverSurvey(parent, this, condition);
	rs->setPosition(condition->position());
	rs->setDefaultMapper();
	return rs;
}

QString RawDataRiverSurveyCreator::name(unsigned int index)
{
	return QString("riversurvey%1").arg(index);
}

QString RawDataRiverSurveyCreator::defaultCaption(unsigned int index)
{
	return QString(tr("RiverSurvey %1")).arg(index);
}

RawData* RawDataRiverSurveyCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	RawData* rawdata = RawDataCreator::restore(node, parent, condition);
	if (rawdata != nullptr) {return rawdata;}
	QDomElement elem = node.toElement();
	QString name = elem.attribute("name");
	if (name.contains("riversurvey")) {
		return create(parent, condition);
	}
	return nullptr;
}

bool RawDataRiverSurveyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition)
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::CellCenter) {return false;}
	if (condition->isOption()) {return false;}
	if (condition->dimensions().size() != 0) {return false;}
	return true;
}
