#ifndef GEODATAFACTORY_H
#define GEODATAFACTORY_H

#include "../../guicore_global.h"

class GeoData;
class GeoDataCreator;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class GUICOREDLL_EXPORT GeoDataFactory
{
private:
	GeoDataFactory();

public:
	static GeoDataFactory& instance();

	void addCreator(GeoDataCreator* creator);

	GeoDataCreator* creator(const QString& typeName);
	const std::vector<GeoDataCreator*>& creators() const;
	std::vector<GeoDataCreator*> compatibleCreators(SolverDefinitionGridAttribute* condition) const;

	void setIsSetup(bool setup);
	bool isSetup() const;

	GeoData* restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const;

private:
	std::vector<GeoDataCreator*> m_creators;
	bool m_isSetup;
	static GeoDataFactory* m_instance;
};

#endif // GEODATAFACTORY_H
