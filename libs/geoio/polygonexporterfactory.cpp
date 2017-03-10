#include "polygonexporterfactory.h"
#include "private/polygonexporterfactory_impl.h"
#include "polygoncsvexporter.h"
#include "polygonshapeexporter.h"

PolygonExporterFactory* PolygonExporterFactory::m_instance = nullptr;

PolygonExporterFactory& PolygonExporterFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new PolygonExporterFactory();
	}
	return *m_instance;
}

PolygonExporterFactory::PolygonExporterFactory() :
	impl {new Impl()}
{
	impl->m_exporters.push_back(new PolygonShapeExporter());
	impl->m_exporters.push_back(new PolygonCsvExporter());
}

PolygonExporterFactory::~PolygonExporterFactory()
{
	for (auto i : impl->m_exporters) {
		delete i;
	}

	delete impl;
}

std::vector<PolygonExporterI*> PolygonExporterFactory::exporters() const
{
	return impl->m_exporters;
}
