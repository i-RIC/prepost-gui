#include "geodatanetcdf.h"
#include "geodatanetcdfcreator.h"
#include "geodatanetcdfimporter.h"
//#include "geodatanetcdfimporter.h"
//#include "geodatanetcdfshapeexporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataNetcdfCreator::GeoDataNetcdfCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Raster Data")}
{}

QString GeoDataNetcdfCreator::name(unsigned int index) const
{
	return QString("rasterdata%1").arg(index);
}

QString GeoDataNetcdfCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Raster Data%1")).arg(index);
}

bool GeoDataNetcdfCreator::requestCoordinateSystem() const
{
	return true;
}
