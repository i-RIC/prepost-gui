#ifndef GEODATAFACTORY_H
#define GEODATAFACTORY_H

#include <guicore/pre/geodata/geodatafactoryinterface.h>

#include <vector>

class QDomNode;
class GeoData;
class GeoDataCreator;
class ProjectDataItem;
class SolverDefinitionGridAttribute;

class GeoDataFactory : public GeoDataFactoryInterface
{
private:
	GeoDataFactory();

public:
	static GeoDataFactory& instance();

	const std::vector<GeoDataCreator*>& creators() const override;
	std::vector<GeoDataCreator*> compatibleCreators(SolverDefinitionGridAttribute* condition) const override;

	GeoData* restore(const QDomNode& node, ProjectDataItem* item, SolverDefinitionGridAttribute* cond) const;

private:
	std::vector<GeoDataCreator*> m_creators;
	static GeoDataFactory* m_instance;
};

#endif // GEODATAFACTORY_H
