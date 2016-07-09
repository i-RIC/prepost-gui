#include "polylineexporterfactory.h"
#include "private/polylineexporterfactory_impl.h"

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

}

PolylineExporterFactory::~PolylineExporterFactory()
{
	delete impl;
}

std::vector<PolylineExporterI*> PolylineExporterFactory::exporters() const
{
	return impl->m_exporters;
}
