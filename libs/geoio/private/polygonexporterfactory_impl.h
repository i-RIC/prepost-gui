#ifndef POLYGONEXPORTERFACTORY_IMPL_H
#define POLYGONEXPORTERFACTORY_IMPL_H

#include "../polygonexporterfactory.h"

class PolygonExporterFactory::Impl
{
public:
	std::vector<PolygonExporterI*> m_exporters;
};

#endif // POLYGONEXPORTERFACTORY_IMPL_H
