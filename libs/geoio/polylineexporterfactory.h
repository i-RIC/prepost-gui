#ifndef POLYLINEEXPORTERFACTORY_H
#define POLYLINEEXPORTERFACTORY_H

#include "geoio_global.h"

#include <vector>

class PolylineExporterI;

class GEOIODLL_EXPORT PolylineExporterFactory
{
public:
	static PolylineExporterFactory& instance();

	std::vector<PolylineExporterI*> exporters() const;

private:
	static PolylineExporterFactory* m_instance;

	PolylineExporterFactory();
	~PolylineExporterFactory();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polylineexporterfactory_impl.h"
#endif // _DEBUG

#endif // POLYLINEEXPORTERFACTORY_H
