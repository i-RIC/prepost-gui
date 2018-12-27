#include "hydraulicdatacreator.h"
#include "hydraulicdataexporter.h"
#include "hydraulicdataimporter.h"
#include "hydraulicdatamultiexporter.h"

#include "private/hydraulicdatacreator_impl.h"

HydraulicDataCreator::HydraulicDataCreator(const std::string& typeName, const QString& caption) :
	impl {new Impl{}}
{
	impl->m_typeName = typeName;
	impl->m_caption = caption;
}

HydraulicDataCreator::~HydraulicDataCreator()
{
	for (auto importer : impl->m_importers) {
		delete importer;
	}
	for (auto exporter : impl->m_exporters) {
		delete exporter;
	}
	for (auto exporter : impl->m_multiExporters) {
		delete exporter;
	}

	delete impl;
}

const std::string& HydraulicDataCreator::typeName() const
{
	return impl->m_typeName;
}

const QString& HydraulicDataCreator::caption() const
{
	return impl->m_caption;
}

const std::vector<HydraulicDataImporter*>& HydraulicDataCreator::importers() const
{
	return impl->m_importers;
}

std::vector<HydraulicDataImporter*>& HydraulicDataCreator::importers()
{
	return impl->m_importers;
}

const std::vector<HydraulicDataExporter*>& HydraulicDataCreator::exporters() const
{
	return impl->m_exporters;
}

std::vector<HydraulicDataExporter*>& HydraulicDataCreator::exporters()
{
	return impl->m_exporters;
}

const std::vector<HydraulicDataMultiExporter*>& HydraulicDataCreator::multiExporters() const
{
	return impl->m_multiExporters;
}

std::vector<HydraulicDataMultiExporter*>& HydraulicDataCreator::multiExporters()
{
	return impl->m_multiExporters;
}
