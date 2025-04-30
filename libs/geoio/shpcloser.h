#ifndef SHPCLOSER_H
#define SHPCLOSER_H

#include "geoio_global.h"

#include <shapefil.h>

class GEOIODLL_EXPORT SHPCloser
{
public:
	SHPCloser(SHPHandle handle);
	~SHPCloser();

private:
	SHPHandle m_handle;
};

#endif // SHPCLOSER_H
