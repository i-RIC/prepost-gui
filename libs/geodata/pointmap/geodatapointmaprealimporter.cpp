#include "geodatapointmaprealimporter.h"
#include "geodatapointmaprealimporterfilterdialog.h"
#include "geodatapointmapt.h"

#include <misc/stringtool.h>

#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertex.h>

#include <gdal_priv.h>

GeoDataPointmapRealImporter::GeoDataPointmapRealImporter(GeoDataCreator* creator) :
	GeoDataImporter("tpo", tr("DEM etc. (*.tpo, *.stl)"), creator),
	poDataset(NULL),
	poBand(NULL)
{}

bool GeoDataPointmapRealImporter::doInit(const QString& /*filename*/, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* /*item*/, QWidget* w)
{
	GeoDataPointmapRealImporterFilterDialog filterDialog(w);
	if (filterDialog.exec() != QDialog::Accepted) {
		return false;
	}
	m_filterValue = filterDialog.filterValue();
	return true;
}

bool GeoDataPointmapRealImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	QFileInfo finfo(filename());
	// Allocate objects to hold points and vertex cells.
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkDoubleArray> values = vtkSmartPointer<vtkDoubleArray>::New();

	double xt[3];
	auto filter = selectedFilter();
	if (filter == tr("Topography File (*.tpo)") || finfo.suffix() == "tpo")
	{
		QFile file(filename());
		if (! file.open(QIODevice::ReadOnly)) {
			QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename()));
			return false;
		}
		QTextStream in(&file);
		int index;
		double x[3];
		int counter = 0;
		int filterCounter = 0;

		QMultiMap<double, double>* map = new QMultiMap<double, double>;
		do {
			QString qstr = in.readLine();
			qstr.replace(",", " ");

			auto frags = qstr.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			if (frags.size() < 3) {continue;}

			QTextStream in2(&qstr, QIODevice::ReadOnly);
			if (frags.size() == 3) {
				in2 >> x[0] >> x[1] >> x[2];
			} else if (frags.size() == 4) {
				in2 >> index >> x[0] >> x[1] >> x[2];
			}
			if (map->contains(x[0], x[1])) {continue;}

			if (in.status() == QTextStream::Ok && filterCounter == 0) {
				xt[0] = x[0]; xt[1] = x[1]; xt[2] = 0;
				points->InsertNextPoint(xt);
				values->InsertNextValue(x[2]);
				map->insert(x[0], x[1]);
				counter++;
			}
			++ filterCounter;
			if (filterCounter == m_filterValue) {
				filterCounter = 0;
			}
			if (in.status() == QTextStream::ReadCorruptData) {
				return false;
			}
		} while (! in.atEnd());
		map->clear();
		delete map;
		if (points->GetNumberOfPoints() < 3) {return false;}
		file.close();
	} else if (filter == tr("USGS NED (*.adf)") || finfo.suffix() == "adf") {
		// read USGS National Elevation Data (NED) data format.
		// Data available through http://seamless.usgs.gov/
		// filename will be "w001001.adf" always .adf format is read using GDAL library
		// available at http://www.gdal.org/
		{
			GDALAllRegister();

			// Following variables for debug
			// QVector<double> xloc, yloc, zloc;
			// QVector<double> lat, lon;
			this->poDataset = (GDALDataset*) GDALOpen(filename().toLocal8Bit().constData(), GA_ReadOnly);
			if (poDataset == NULL) {
				std::cerr << "Cannot open file NED adf file for reading" << std::endl;
				return false;
			}
			double adfGeoTransform[6];
			if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None) {
				QString* txt();

				//sprintf_s(str, "Driver: %s/%s\n",
				//		poDataset->GetDriver()->GetDescription(),
				//		poDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );
				//p_MainWindow->textEdit->appendPlainText(QString(str));

				//sprintf_s(str, "Size is %dx%dx%d\n",
				//		poDataset->GetRasterXSize(), poDataset->GetRasterYSize(),
				//		poDataset->GetRasterCount() );
				//p_MainWindow->textEdit->appendPlainText(QString(str));

				//if( poDataset->GetProjectionRef()  != NULL )
				//    sprintf_s( str, "Projection is `%s'\n", poDataset->GetProjectionRef() );

				//if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
				//{
				//	sprintf_s( str, "Origin = (%.6f,%.6f)\n",
				//			adfGeoTransform[0], adfGeoTransform[3] );
				//	p_MainWindow->textEdit->appendPlainText(QString(str));

				//	sprintf_s(str,  "Pixel Size = (%.6f,%.6f)\n",
				//			adfGeoTransform[1], adfGeoTransform[5] );
				//	p_MainWindow->textEdit->appendPlainText(QString(str));
				//}

				int nBlockXSize, nBlockYSize;
				int bGotMin, bGotMax;
				double adfMinMax[2];

				this->poBand = this->poDataset->GetRasterBand(1);
				this->poBand->GetBlockSize(&nBlockXSize, &nBlockYSize);

				//sprintf_s(str, "Block=%dx%d Type=%s, ColorInterp=%s\n",
				//			nBlockXSize, nBlockYSize,
				//			GDALGetDataTypeName(poBand->GetRasterDataType()),
				//			GDALGetColorInterpretationName(
				//				poBand->GetColorInterpretation()) );
				//p_MainWindow->textEdit->appendPlainText(QString(str));

				adfMinMax[0] = poBand->GetMinimum(&bGotMin);
				adfMinMax[1] = poBand->GetMaximum(&bGotMax);
				if (!(bGotMin && bGotMax)) {
					GDALComputeRasterMinMax((GDALRasterBandH)poBand, TRUE, adfMinMax);
				}

				//sprintf_s(str, "Min=%.3fd, Max=%.3f\n", adfMinMax[0], adfMinMax[1] );
				//p_MainWindow->textEdit->appendPlainText(QString(str));

				// GDALDataType dataType = poBand->GetRasterDataType();
				// const char* projRef = poDataset->GetProjectionRef();
				// const char* projgcp = poDataset->GetGCPProjection();

				// double* pafScanline;
				int   nXSize = poBand->GetXSize();
				int	  nYSize = poBand->GetYSize();
				double InPixel;
				//double Xp, Yp;

				// The following section uses GDAL tools for changing the projection of the dataset
				// Here it is changed from Lat/Lon to UTM (must supply a code)
				// Code was tested succesfully, but requires an additional library "Proj.4 available at
				// http://trac.osgeo.org/proj/  GDAL must be recompiled with Proj.4 flags set in nmake.opt file
				// Maybe useful in the future. At this time we use some other code to do transformations from
				// Lat/Lon to UTM.

				// const char* pszProjection = this->poDataset->GetProjectionRef();
				// char* pszPrettyWkt, *pszPrettyWkt2;
				//	int err = this->oSourceSRS.SetFromUserInput(pszProjection);
				//	err = this->oSourceSRS.exportToPrettyWkt(&pszPrettyWkt);
				//	p_MainWindow->textEdit->appendPlainText(QString(pszPrettyWkt));
				//	err = this->oTargetSRS.importFromEPSG(32613);
				//	err = this->oTargetSRS.exportToPrettyWkt(&pszPrettyWkt2);
				//	p_MainWindow->textEdit->appendPlainText(QString(pszPrettyWkt2));

				//OGRFree(pszPrettyWkt);
				//OGRFree(pszPrettyWkt2);

				// Files for debugging
				// Open the input file.
				//QFile file("georefout.tpo");
				//if(!file.open(QIODevice::WriteOnly)) {
				//}
				//QTextStream out(&file);
				//out.setRealNumberNotation(QTextStream::ScientificNotation);
				//out.setRealNumberPrecision(12);

				//QFile file2("georeflatlonout.xyz");
				//if(!file2.open(QIODevice::WriteOnly)) {
				//}
				//QTextStream out2(&file2);

				double easting = 0, northing = 0;
				// Not currently using GDAL Coordinate Transformation routines
				//this->poCT = OGRCreateCoordinateTransformation(&this->oSourceSRS, &this->oTargetSRS);
				double tXp, tYp;

				Q_UNUSED(tXp)
				Q_UNUSED(tYp)

				//debug output
				//out << nXSize*nYSize << "\n";
				int counter = 0;
				for (int i = 0; i < nXSize; i++) {
					for (int j= 0; j < nYSize; j++) {
						poBand->RasterIO(GF_Read, i, j, 1, 1, &InPixel, 1, 1, GDT_Float32, 0, 0);
						tXp = adfGeoTransform[0] + i*adfGeoTransform[1] + j*adfGeoTransform[2];
						tYp = adfGeoTransform[3] + i*adfGeoTransform[4] + j*adfGeoTransform[5];
						//LatLonToUtmWGS84 (utmXZone, utmYZone, easting, northing, tYp, tXp);
						xt[0]=easting; xt[1]=northing; xt[2]=0;
						points->InsertNextPoint(xt);
						values->InsertNextValue(InPixel);
						counter++;
					}
				}
			} else if (this->poDataset->GetGCPCount() > 0) {
				//int gcpCount = this->poDataset->GetGCPCount();
				//const GDAL_GCP* gcp = this->poDataset->GetGCPs();
				//double pixel;
				//double line;
				//double gcpx;
				//double gcpy;
				//double gcpz;

				//for(int i = 0; i < gcpCount; i++) {
				//	line = gcp[i].dfGCPLine;
				//	pixel = gcp[i].dfGCPPixel;
				//	gcpx = gcp[i].dfGCPX;
				//	gcpy = gcp[i].dfGCPY;
				//	gcpz = gcp[i].dfGCPZ;
				//}

				std::cerr << "Format of NED .adf file not currently supported" << std::endl;
				return false;

			} else {
				std::cerr <<  "NED .adf file not currently supported" << std::endl;
				return false;
			}
		}
	}
	pmap->setPoints(points, values);
	pmap->doDelaunay();
	return true;
}

const QStringList GeoDataPointmapRealImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Topography File (*.tpo)");
	ret << tr("USGS NED (*.adf)");
	return ret;
}

const QStringList GeoDataPointmapRealImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "tpo" << "adf";
	return ret;
}
