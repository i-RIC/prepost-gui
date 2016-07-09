#ifndef POLYLINEEXPORTERFACTORY_IMPL_H
#define POLYLINEEXPORTERFACTORY_IMPL_H

#include "../polylineexporterfactory.h"

class PolylineExporterFactory::Impl
{
public:
	std::vector<PolylineExporterI*> m_exporters;
};

#endif // POLYLINEEXPORTERFACTORY_IMPL_H
