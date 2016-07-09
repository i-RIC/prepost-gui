#ifndef POLYGONEXPORTERFACTORY_H
#define POLYGONEXPORTERFACTORY_H

#include "geoio_global.h"

#include <vector>

class PolygonExporterI;

class GEOIODLL_EXPORT PolygonExporterFactory
{
public:
	static PolygonExporterFactory& instance();

	std::vector<PolygonExporterI*> exporters() const;

private:
	static PolygonExporterFactory* m_instance;

	PolygonExporterFactory();
	~PolygonExporterFactory();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polygonexporterfactory_impl.h"
#endif // _DEBUG

#endif // POLYGONEXPORTERFACTORY_H
