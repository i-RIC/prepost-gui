#include "../../datamodel/graphicswindowdataitem.h"
#include "../../project/projectdata.h"
#include "../base/preprocessorgeodatadataitemi.h"
#include "../geodatabackground/geodatabackground.h"
#include "geodata.h"
#include "geodatacreator.h"
#include "private/geodatacreator_impl.h"

#include <QDomElement>
#include <QDomNode>
#include <QFile>

#include <unordered_set>

GeoDataCreator::GeoDataCreator(const QString& typeName, const QString& caption) :
	QObject(nullptr),
	impl {new Impl {}}
{
	impl->m_typeName = typeName;
	impl->m_caption = caption;
}

GeoDataCreator::~GeoDataCreator()
{}

QString GeoDataCreator::name(unsigned int) const
{
	return "";
}

const QString& GeoDataCreator::typeName() const
{
	return impl->m_typeName;
}

const QString& GeoDataCreator::caption() const
{
	return impl->m_caption;
}

GeoData* GeoDataCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("type") != typeName()) {return nullptr;}

	return create(parent, condition);
}

void GeoDataCreator::setNameAndDefaultCaption(const std::vector<GraphicsWindowDataItem *> &list, GeoData* data, ProjectData* projectData)
{
	std::unordered_set<QString> nameSet;
	std::unordered_set<QString> captionSet;

	// first, setup nameSet and captionSet.
	for (auto item : list) {
		auto geodata = dynamic_cast<PreProcessorGeoDataDataItemI*>(item)->geoData();
		if (dynamic_cast<GeoDataBackground*> (geodata) != nullptr) {continue;}

		if (geodata->name() != "") {nameSet.insert(geodata->name());}
		if (geodata->caption() != "") {captionSet.insert(geodata->caption());}
	}

	unsigned int idx = 1;
	QDir workDir(projectData->workDirectory());
	while (true) {
		QString tmpName = data->creator()->name(idx);
		if (nameSet.find(tmpName) != nameSet.end()) {
			++idx;
			continue;
		}
		data->setName(tmpName);
		QFile f(workDir.absoluteFilePath(data->relativeFilename()));
		if (f.exists()) {
			++idx;
			continue;
		}
		break;
	}

	// set caption
	idx = 1;

	while (true) {
		QString tmpCaption = data->creator()->defaultCaption(idx);
		if (captionSet.find(tmpCaption) != captionSet.end()) {
			++idx;
			continue;
		}

		data->setCaption(tmpCaption);
		break;
	}
}

bool GeoDataCreator::requestCoordinateSystem() const
{
	return false;
}

const std::vector<GeoDataImporter*>& GeoDataCreator::importers() const
{
	return impl->m_importers;
}

std::vector<GeoDataImporter*>& GeoDataCreator::importers()
{
	return impl->m_importers;
}

const std::vector<GeoDataWebImporter*>& GeoDataCreator::webImporters() const
{
	return impl->m_webImporters;
}

std::vector<GeoDataWebImporter*>& GeoDataCreator::webImporters()
{
	return impl->m_webImporters;
}

const std::vector<GeoDataExporter*>& GeoDataCreator::exporters() const
{
	return impl->m_exporters;
}

std::vector<GeoDataExporter*>& GeoDataCreator::exporters()
{
	return impl->m_exporters;
}

bool GeoDataCreator::isCreatable() const
{
	return false;
}
