#include "polylineexporterfactory.h"
#include "private/polylineexporterfactory_impl.h"
#include "polylinecsvexporter.h"
#include "polylineshapeexporter.h"

PolylineExporterFactory* PolylineExporterFactory::m_instance = nullptr;

PolylineExporterFactory& PolylineExporterFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new PolylineExporterFactory();
	}
	return *m_instance;
}

PolylineExporterFactory::PolylineExporterFactory() :
	impl {new Impl()}
{
	impl->m_exporters.push_back(new PolylineShapeExporter());
	impl->m_exporters.push_back(new PolylineCsvExporter());
}

PolylineExporterFactory::~PolylineExporterFactory()
{
	for (auto i : impl->m_exporters) {
		delete i;
	}

	delete impl;
}

std::vector<PolylineExporterI*> PolylineExporterFactory::exporters() const
{
	return impl->m_exporters;
}
