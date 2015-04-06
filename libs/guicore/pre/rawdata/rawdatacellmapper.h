#ifndef RAWDATACELLMAPPER_H
#define RAWDATACELLMAPPER_H

#include "rawdatamapper.h"

class RawDataCellMapper : public RawDataMapper
{
public:
	RawDataCellMapper(RawData* parent) : RawDataMapper(parent){}
	virtual bool map(unsigned int cellIndex) = 0;
};

#endif // RAWDATACELLMAPPER_H
