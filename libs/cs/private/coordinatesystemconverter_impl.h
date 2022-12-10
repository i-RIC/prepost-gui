#ifndef COORDINATESYSTEMCONVERTER_IMPL_H
#define COORDINATESYSTEMCONVERTER_IMPL_H

#include "../coordinatesystemconverter.h"

#include <proj_api.h>

class CoordinateSystemConverter::Impl
{
public:
	projPJ m_sourcePj;
	projPJ m_targetPj;
};

#endif // COORDINATESYSTEMCONVERTER_IMPL_H
