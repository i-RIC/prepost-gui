#include "geodatanetcdfncexporter.h"

#include <guicore/pre/geodata/geodata.h>

#include <QFile>

GeoDataNetcdfNcExporter::GeoDataNetcdfNcExporter(GeoDataCreator* creator) :
	GeoDataExporter {"NC", creator}
{}

bool GeoDataNetcdfNcExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* /*pd*/)
{
	// just copy the file
	return QFile::copy(data->filename(), filename);
}

const QStringList GeoDataNetcdfNcExporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("NetCDF files (*.nc)"));
	return ret;
}
