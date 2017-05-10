#include "polygonimporterfactory.h"
#include "private/polygonimporterfactory_impl.h"
#include "polygoncsvimporter.h"
#include "polygonshapeimporter.h"

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
	impl->m_importers.push_back(new PolygonShapeImporter());
	impl->m_importers.push_back(new PolygonCsvImporter());
}

PolygonImporterFactory::~PolygonImporterFactory()
{
	for (auto i : impl->m_importers) {
		delete i;
	}

	delete impl;
}

std::vector<PolygonImporterI*> PolygonImporterFactory::importers() const
{
	return impl->m_importers;
}
