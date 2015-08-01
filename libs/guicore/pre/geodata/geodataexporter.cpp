#include "geodatacreator.h"
#include "geodataexporter.h"

GeoDataExporter::GeoDataExporter(GeoDataCreator* creator) :
	QObject {creator}
{}

GeoDataCreator* GeoDataExporter::creator() const
{
	return dynamic_cast<GeoDataCreator*>(parent());
}
