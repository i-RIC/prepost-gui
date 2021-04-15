#include "geodatanetcdf.h"
#include "geodatanetcdftimeseriescreator.h"

GeoDataNetcdfTimeSeriesCreator::GeoDataNetcdfTimeSeriesCreator(const QString& typeName) :
    GeoDataCreator {typeName, tr("Time Series Raster Data")}
{}

QString GeoDataNetcdfTimeSeriesCreator::name(unsigned int index) const
{
    return QString("timeseriesrasterdata%1").arg(index);
}

QString GeoDataNetcdfTimeSeriesCreator::defaultCaption(unsigned int index) const
{
    return QString(tr("Time Series Raster Data%1")).arg(index);
}
