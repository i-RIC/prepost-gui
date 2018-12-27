#ifndef HYDRAULICDATACREATOR_IMPL_H
#define HYDRAULICDATACREATOR_IMPL_H

#include "../hydraulicdatacreator.h"

#include <QString>

#include <string>

class HydraulicDataCreator::Impl
{
public:
	std::string m_typeName;
	QString m_caption;

	std::vector<HydraulicDataImporter*> m_importers;
	std::vector<HydraulicDataExporter*> m_exporters;
	std::vector<HydraulicDataMultiExporter*> m_multiExporters;
};

#endif // HYDRAULICDATACREATOR_IMPL_H
