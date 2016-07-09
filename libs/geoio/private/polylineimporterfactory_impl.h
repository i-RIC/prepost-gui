#ifndef POLYLINEIMPORTERFACTORY_IMPL_H
#define POLYLINEIMPORTERFACTORY_IMPL_H

#include "../polylineimporterfactory.h"

class PolylineImporterFactory::Impl
{
public:
	std::vector<PolylineImporterI*> m_importers;
};

#endif // POLYLINEIMPORTERFACTORY_IMPL_H
