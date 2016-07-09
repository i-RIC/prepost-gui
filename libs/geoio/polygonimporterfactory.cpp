#include "polygonimporterfactory.h"
#include "private/polygonimporterfactory_impl.h"

PolygonImporterFactory* PolygonImporterFactory::m_instance = nullptr;

PolygonImporterFactory& PolygonImporterFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new PolygonImporterFactory();
	}
	return *m_instance;
}

PolygonImporterFactory::PolygonImporterFactory() :
	impl {new Impl()}
{

}

PolygonImporterFactory::~PolygonImporterFactory()
{
	delete impl;
}

std::vector<PolygonImporterI*> PolygonImporterFactory::importers() const
{
	return impl->m_importers;
}
