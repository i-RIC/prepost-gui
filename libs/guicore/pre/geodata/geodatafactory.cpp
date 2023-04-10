#include "geodatacreator.h"
#include "geodatafactory.h"

GeoDataFactory* GeoDataFactory::m_instance = nullptr;

GeoDataFactory::GeoDataFactory() :
	m_isSetup {false}
{}

GeoDataFactory& GeoDataFactory::instance()
{
	if (m_instance == nullptr) {
		m_instance = new GeoDataFactory();
	}

	return *m_instance;
}

void GeoDataFactory::addCreator(GeoDataCreator* creator)
{
	m_creators.push_back(creator);
}

GeoDataCreator* GeoDataFactory::creator(const QString& typeName)
{
	for (auto c : m_creators) {
		if (c->typeName() == typeName) {
			return c;
		}
	}
	return nullptr;
}

const std::vector<GeoDataCreator*>& GeoDataFactory::creators() const
{
	return m_creators;
}

std::vector<GeoDataCreator*> GeoDataFactory::compatibleCreators(SolverDefinitionGridAttribute* condition) const
{
	std::vector<GeoDataCreator*> ret;
	for (auto c : m_creators) {
		if (! c->isCompatibleWith(condition)) {continue;}

		ret.push_back(c);
	}
	return ret;
}

void GeoDataFactory::setIsSetup(bool setup)
{
	m_isSetup = setup;
}

bool GeoDataFactory::isSetup() const
{
	return m_isSetup;
}

GeoData* GeoDataFactory::restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const
{
	for (GeoDataCreator* c : m_creators) {
		GeoData* ret = c->restore(node, item, cond);
		if (ret) {return ret;}
	}
	return nullptr;
}
