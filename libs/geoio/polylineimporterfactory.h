#ifndef POLYLINEIMPORTERFACTORY_H
#define POLYLINEIMPORTERFACTORY_H

#include "geoio_global.h"

#include <vector>

class PolylineImporterI;

class GEOIODLL_EXPORT PolylineImporterFactory
{
public:
	static PolylineImporterFactory& instance();

	std::vector<PolylineImporterI*> importers() const;

private:
	static PolylineImporterFactory* m_instance;

	PolylineImporterFactory();
	~PolylineImporterFactory();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/polylineimporterfactory_impl.h"
#endif // _DEBUG

#endif // POLYLINEIMPORTERFACTORY_H
