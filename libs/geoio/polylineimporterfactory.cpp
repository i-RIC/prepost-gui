#include "polylineimporterfactory.h"
#include "private/polylineimporterfactory_impl.h"

PolylineImporterFactory* PolylineImporterFactory::m_instance = nullptr;

PolylineImporterFactory& PolylineImporterFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new PolylineImporterFactory();
	}
	return *m_instance;
}

PolylineImporterFactory::PolylineImporterFactory() :
	impl {new Impl()}
{

}

PolylineImporterFactory::~PolylineImporterFactory()
{
	delete impl;
}

std::vector<PolylineImporterI*> PolylineImporterFactory::importers() const
{
	return impl->m_importers;
}
