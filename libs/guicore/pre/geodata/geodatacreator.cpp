#include "../../datamodel/graphicswindowdataitem.h"
#include "../base/preprocessorgeodatadataitemi.h"
#include "geodata.h"
#include "geodatacreator.h"
#include "../geodatabackground/geodatabackground.h"
#include "private/geodatacreator_impl.h"

#include <QDomElement>
#include <QDomNode>
#include <QSet>

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

void GeoDataCreator::setNameAndDefaultCaption(const std::vector<GraphicsWindowDataItem *> &list, GeoData* data)
{
	QSet<QString> nameSet;
	QSet<QString> captionSet;

	// first, setup nameSet and captionSet.
	for (auto it = list.begin(); it != list.end(); ++it) {
		GeoData* geodata = dynamic_cast<PreProcessorGeoDataDataItemI*>(*it)->geoData();
		if (dynamic_cast<GeoDataBackground*> (geodata) != nullptr) {continue;}

		if (geodata->name() != "") {nameSet.insert(geodata->name());}
		if (geodata->caption() != "") {captionSet.insert(geodata->caption());}
	}
	unsigned int idx = 1;
	bool ok = false;
	while (! ok) {
		QString tmpname = data->creator()->name(idx);
		QString tmpcap = data->creator()->defaultCaption(idx);
		if (! nameSet.contains(tmpname) && ! captionSet.contains(tmpcap)) {
			data->setName(tmpname);
			data->setCaption(tmpcap);
			ok = true;
		}
		++idx;
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
