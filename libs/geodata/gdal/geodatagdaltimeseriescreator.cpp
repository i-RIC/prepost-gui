#include "geodatagdal.h"
#include "geodatagdaltimeseriescreator.h"

GeoDataGdalTimeSeriesCreator::GeoDataGdalTimeSeriesCreator(const QString& typeName) :
    GeoDataCreator {typeName, tr("Time Series Raster Data")}
{}

QString GeoDataGdalTimeSeriesCreator::name(unsigned int index) const
{
    return QString("timeseriesrasterdata%1").arg(index);
}

QString GeoDataGdalTimeSeriesCreator::defaultCaption(unsigned int index) const
{
    return QString(tr("Time Series Raster Data%1")).arg(index);
}

bool GeoDataGdalTimeSeriesCreator::requestCoordinateSystem() const
{
    return true;
}
