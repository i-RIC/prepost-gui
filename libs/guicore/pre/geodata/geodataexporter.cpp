#include "geodatacreator.h"
#include "geodataexporter.h"
#include "private/geodataexporter_impl.h"

GeoDataExporter::GeoDataExporter(const QString &caption, GeoDataCreator* creator) :
	QObject {creator},
	impl {new Impl {}}
{
	impl->m_caption = caption;
}

GeoDataExporter::~GeoDataExporter()
{
	delete impl;
}

const QString& GeoDataExporter::caption() const
{
	return impl->m_caption;
}

GeoDataCreator* GeoDataExporter::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}
