#include "geodatagdal.h"
#include "geodatagdalcreator.h"
#include "geodatagdalnetcdfimporter.h"

#include <guibase/irictoolbar.h>

#include <QAction>
#include <QIcon>

GeoDataGdalCreator::GeoDataGdalCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("Raster Data")}
{}

QString GeoDataGdalCreator::name(unsigned int index) const
{
	return QString("rasterdata%1").arg(index);
}

QString GeoDataGdalCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Raster Data%1")).arg(index);
}

bool GeoDataGdalCreator::requestCoordinateSystem() const
{
	return true;
}
