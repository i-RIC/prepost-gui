#include "structured2dgridgdalimporter.h"

#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainert.h>
#include <misc/stringtool.h>

#include <QObject>

#include <gdal_priv.h>

#include <cmath>
#include <vector>

Structured2DGridGdalImporter::Structured2DGridGdalImporter()
	: QObject(), GridImporterInterface()
{

}

QString Structured2DGridGdalImporter::caption() const
{
	return tr("GDAL supported files");
}

QStringList Structured2DGridGdalImporter::fileDialogFilters() const
{
	QStringList ret;
	ret.append(tr("GeoTiff files(*.tif)"));
	ret.append(tr("ArcInfo ASCII files(*.asc)"));
	return ret;
}

SolverDefinitionGridType::GridType Structured2DGridGdalImporter::supportedGridType() const
{
	return SolverDefinitionGridType::gtStructured2DGrid;
}

bool Structured2DGridGdalImporter::import(Grid* grid, const QString& filename, const QString& /*selectedFilter*/, QWidget* parent)
{
	GDALAllRegister();

	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(grid);

	auto dataset = (GDALDataset*)(GDALOpen(iRIC::toStr(filename).c_str(), GA_ReadOnly));
	if (dataset == NULL) {return false;}

	double transform[6];
	CPLErr err = dataset->GetGeoTransform(transform);
	if (err != CE_None) {return false;}

	int xsize = dataset->GetRasterXSize();
	int ysize = dataset->GetRasterYSize();

	grid2d->setDimensions(xsize + 1, ysize + 1);

	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate((xsize + 1) * (ysize + 1));

	for (int j = 0; j <= ysize; ++j) {
		for (int i = 0; i <= xsize; ++i) {
			double x = transform[0] + transform[1] * i;
			double y = transform[3] + transform[5] * (ysize - j);
			double z = 0;
			points->InsertNextPoint(x, y, z);
		}
	}
	grid2d->vtkGrid()->SetPoints(points);
	points->Delete();

	// allocate memory for all grid related conditions.
	auto& atts = grid2d->gridAttributes();
	for (auto& att : atts){
		att->allocate();
	}


	return true;
}
