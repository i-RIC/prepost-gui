#include "polygonexporterfactory.h"
#include "private/polygonexporterfactory_impl.h"

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

}

PolygonExporterFactory::~PolygonExporterFactory()
{
	delete impl;
}

std::vector<PolygonExporterI*> PolygonExporterFactory::exporters() const
{
	return impl->m_exporters;
}
