#ifndef COORDINATESYSTEMCONVERTER_IMPL_H
#define COORDINATESYSTEMCONVERTER_IMPL_H

#include "../coordinatesystemconverter.h"

#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>

class CoordinateSystemConverter::Impl
{
public:
	CoordinateSystem* m_sourceCs;
	CoordinateSystem* m_targetCs;

	projPJ m_sourcePj;
	projPJ m_targetPj;
};

#endif // COORDINATESYSTEMCONVERTER_IMPL_H
