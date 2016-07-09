#ifndef POLYGONIMPORTERFACTORY_IMPL_H
#define POLYGONIMPORTERFACTORY_IMPL_H

#include "../polygonimporterfactory.h"

class PolygonImporterFactory::Impl
{
public:
	std::vector<PolygonImporterI*> m_importers;
};

#endif // POLYGONIMPORTERFACTORY_IMPL_H
