#ifndef GEODATAFACTORYINTERFACE_H
#define GEODATAFACTORYINTERFACE_H

#include <vector>

class GeoDataCreator;
class SolverDefinitionGridAttribute;

class GeoDataFactoryInterface
{
public:
	virtual ~GeoDataFactoryInterface() {}

	virtual const std::vector<GeoDataCreator*>& creators() const = 0;
	virtual std::vector<GeoDataCreator*> compatibleCreators(SolverDefinitionGridAttribute* condition) const = 0;
};

#endif // GEODATAFACTORYINTERFACE_H
