#include "geodatacreator.h"
#include "geodataimporter.h"

GeoDataImporter::GeoDataImporter(GeoDataCreator* creator)
	: QObject(creator)
{}

GeoDataCreator* GeoDataImporter::creator()
{
	return dynamic_cast<GeoDataCreator*>(parent());
}

bool GeoDataImporter::importInit(const QString& filename, const QString& selectedFilter, int* count, SolverDefinitionGridAttribute* condition, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	m_filename = filename;
	m_selectedFilter = selectedFilter;
	*count = 1;
	return doInit(filename, selectedFilter, count, condition, item, w);
}

bool GeoDataImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* /*w*/)
{
	return true;
}
