#include "geodatacreator.h"
#include "geodataexporter.h"

GeoDataExporter::GeoDataExporter(GeoDataCreator* creator)
	: QObject(creator)
{}

GeoDataCreator* GeoDataExporter::creator()
{
	return dynamic_cast<GeoDataCreator*>(parent());
}
