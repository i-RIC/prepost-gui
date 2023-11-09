#include "structured2dgridgdalimporter.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainert.h>
#include <misc/stringtool.h>

#include <QObject>

#include <gdal_priv.h>

#include <cmath>
#include <vector>

Structured2DGridGdalImporter::Structured2DGridGdalImporter()
	: QObject(), GridImporterI()
{

}

QString Structured2DGridGdalImporter::caption() const
{
	return tr("GDAL supported files");
}

QStringList Structured2DGridGdalImporter::fileDialogFilters() const
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif *.tiff)"));
	ret.append(tr("ArcInfo ASCII files(*.asc)"));
	return ret;
}

SolverDefinitionGridType::GridType Structured2DGridGdalImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2DGridGdalImporter::import(v4InputGrid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	GDALAllRegister();

	auto grid2d = dynamic_cast<v4Structured2dGrid*> (grid->grid());

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {return false;}

	double transform[6];
	CPLErr err = dataset->GetGeoTransform(transform);
	if (err != CE_None) {return false;}

	int xsize = dataset->GetRasterXSize();
	int ysize = dataset->GetRasterYSize();
	GDALClose(dataset);

	grid2d->setDimensions(xsize + 1, ysize + 1);

	for (int j = 0; j <= ysize; ++j) {
		for (int i = 0; i <= xsize; ++i) {
			double x = transform[0] + transform[1] * i;
			double y = transform[3] + transform[5] * (ysize - j);

			grid2d->setPoint2d(i, j, QPointF(x, y));
		}
	}
	grid2d->pointsModified();
	grid->allocateAttributes();

	return true;
}
