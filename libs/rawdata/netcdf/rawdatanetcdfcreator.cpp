#include "rawdatanetcdf.h"
#include "rawdatanetcdfcreator.h"
#include "rawdatanetcdfimporter.h"
//#include "rawdatanetcdfimporter.h"
//#include "rawdatanetcdfshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

RawDataNetcdfCreator::RawDataNetcdfCreator()
	: RawDataCreator()
{
	m_caption = tr("Raster data");
}

QString RawDataNetcdfCreator::name(unsigned int index)
{
	return QString("rasterdata%1").arg(index);
}

QString RawDataNetcdfCreator::defaultCaption(unsigned int index)
{
	return QString(tr("Raster data%1")).arg(index);
}
