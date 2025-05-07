#include "geodatagdalncexporter.h"

#include <guicore/pre/geodata/geodata.h>

#include <QFile>

GeoDataGdalNcExporter::GeoDataGdalNcExporter(GeoDataCreator* creator) :
	GeoDataExporter {"NC", creator}
{}

bool GeoDataGdalNcExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* /*pd*/)
{
	// just copy the file
	return QFile::copy(data->filename(), filename);
}

const QStringList GeoDataGdalNcExporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("NetCDF files (*.nc)"));
	return ret;
}
