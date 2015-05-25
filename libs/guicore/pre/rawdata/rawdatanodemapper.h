#ifndef RAWDATANODEMAPPER_H
#define RAWDATANODEMAPPER_H

#include "rawdatamapper.h"

class RawDataNodeMapper : public RawDataMapper
{
public:
	RawDataNodeMapper(RawData* parent) : RawDataMapper(parent) override {}
	virtual bool map(unsigned int nodeIndex) = 0;
};

#endif // RAWDATANODEMAPPER_H
