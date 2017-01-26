#include "../../datamodel/graphicswindowdataitem.h"
#include "../base/preprocessorgeodatadataiteminterface.h"
#include "geodata.h"
#include "geodatacreator.h"
#include "../geodatabackground/geodatabackground.h"

#include <QDomElement>
#include <QDomNode>
#include <QSet>

GeoDataCreator::GeoDataCreator(const QString& typeName, const QString& caption) :
	QObject(nullptr),
	m_typeName {typeName},
	m_caption {caption}
{}

GeoDataCreator::~GeoDataCreator()
{}

QString GeoDataCreator::name(unsigned int) const
{
	return "";
}

const QString& GeoDataCreator::typeName() const
{
	return m_typeName;
}

const QString& GeoDataCreator::caption() const
{
	return m_caption;
}

GeoData* GeoDataCreator::restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	QDomElement elem = node.toElement();
	if (elem.attribute("type") == m_typeName) {
		return create(parent, condition);
	}
	return nullptr;
}

void GeoDataCreator::setNameAndDefaultCaption(const std::vector<GraphicsWindowDataItem *> &list, GeoData* data)
{
	QSet<QString> nameSet;
	QSet<QString> captionSet;

	// first, setup nameSet and captionSet.
	for (auto it = list.begin(); it != list.end(); ++it) {
		GeoData* geodata = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(*it)->geoData();
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

const QList<GeoDataMapper*>& GeoDataCreator::nodeMappers() const
{
	return m_nodeMappers;
}

QList<GeoDataMapper*>& GeoDataCreator::nodeMappers()
{
	return m_nodeMappers;
}

const QList<GeoDataMapper*>& GeoDataCreator::cellMappers() const
{
	return m_cellMappers;
}

QList<GeoDataMapper*>& GeoDataCreator::cellMappers()
{
	return m_cellMappers;
}

const QList<GeoDataImporter*>& GeoDataCreator::importers() const
{
	return m_importers;
}

QList<GeoDataImporter*>& GeoDataCreator::importers()
{
	return m_importers;
}

const QList<GeoDataWebImporter*>& GeoDataCreator::webImporters() const
{
	return m_webImporters;
}

QList<GeoDataWebImporter*>& GeoDataCreator::webImporters()
{
	return m_webImporters;
}

const QList<GeoDataExporter*>& GeoDataCreator::exporters() const
{
	return m_exporters;
}

QList<GeoDataExporter*>& GeoDataCreator::exporters()
{
	return m_exporters;
}

bool GeoDataCreator::isCreatable() const
{
	return false;
}
