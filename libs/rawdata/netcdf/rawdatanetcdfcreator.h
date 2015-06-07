#ifndef RAWDATANETCDFCREATOR_H
#define RAWDATANETCDFCREATOR_H

#include "rd_netcdf_global.h"
#include <guicore/pre/rawdata/rawdatacreator.h>

class RD_NETCDF_EXPORT RawDataNetcdfCreator : public RawDataCreator
{
	Q_OBJECT

public:
	/// Constructor
	RawDataNetcdfCreator();
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
};

#endif // RAWDATANETCDFCREATOR_H
