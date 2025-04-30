#ifndef DBFCLOSER_H
#define DBFCLOSER_H

#include "geoio_global.h"

#include <shapefil.h>

class GEOIODLL_EXPORT DBFCloser
{
public:
	DBFCloser(DBFHandle handle);
	~DBFCloser();

private:
	DBFHandle m_handle;
};

#endif // DBFCLOSER_H
