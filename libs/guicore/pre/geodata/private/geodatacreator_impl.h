#ifndef GEODATACREATOR_IMPL_H
#define GEODATACREATOR_IMPL_H

#include "../geodatacreator.h"

#include <QString>

class GeoDataCreator::Impl
{
public:
	QString m_caption;
	QString m_typeName;

	std::vector<GeoDataMapper*> m_nodeMappers;
	std::vector<GeoDataMapper*> m_cellMappers;

	std::vector<GeoDataImporter*> m_importers;
	std::vector<GeoDataWebImporter*> m_webImporters;
	std::vector<GeoDataExporter*> m_exporters;
};

#endif // GEODATACREATOR_IMPL_H
