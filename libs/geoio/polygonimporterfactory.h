#ifndef POLYGONIMPORTERFACTORY_H
#define POLYGONIMPORTERFACTORY_H

#include "geoio_global.h"

#include <vector>

class PolygonImporterI;

class GEOIODLL_EXPORT PolygonImporterFactory
{
public:
	static PolygonImporterFactory& instance();

	std::vector<PolygonImporterI*> importers() const;

private:
	static PolygonImporterFactory* m_instance;

	PolygonImporterFactory();
	~PolygonImporterFactory();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polygonimporterfactory_impl.h"
#endif // _DEBUG

#endif // POLYGONIMPORTERFACTORY_H
