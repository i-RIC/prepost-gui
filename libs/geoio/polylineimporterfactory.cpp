#include "polylineimporterfactory.h"
#include "private/polylineimporterfactory_impl.h"
#include "polylinecsvimporter.h"
#include "polylineshapeimporter.h"

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
	impl->m_importers.push_back(new PolylineShapeImporter());
	impl->m_importers.push_back(new PolylineCsvImporter());
}

PolylineImporterFactory::~PolylineImporterFactory()
{
	for (auto i : impl->m_importers) {
		delete i;
	}

	delete impl;
}

std::vector<PolylineImporterI*> PolylineImporterFactory::importers() const
{
	return impl->m_importers;
}
