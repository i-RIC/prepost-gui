#include "geodatapointmapstlimporter.h"
#include "geodatapointmapt.h"

#include <cs/coordinatesystembuilder.h>
#include <cs/coordinatesystemconvertdialog.h>
#include <cs/coordinatesystemconverter.h>
#include <cs/gdalutil.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <vtkCellArray.h>
#include <vtkDoubleArray.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkVertex.h>

GeoDataPointmapSTLImporter::GeoDataPointmapSTLImporter(GeoDataCreator* creator) :
	GeoDataImporter("stl", tr("Stereolithography (STL)"), creator),
	m_converter {nullptr}
{}

GeoDataPointmapSTLImporter::~GeoDataPointmapSTLImporter()
{
	delete m_converter;
}

bool GeoDataPointmapSTLImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemInterface* item, QWidget* w)
{
	bool ok = checkHeader(filename, w);
	if (! ok) {return false;}

	auto projectCs = item->projectData()->mainfile()->coordinateSystem();
	if (projectCs == nullptr) {return true;}

	auto csBuilder = item->projectData()->mainWindow()->coordinateSystemBuilder();
	CoordinateSystemConvertDialog dialog(w);
	dialog.setBuilder(csBuilder);
	dialog.setEnabled(true);

	auto prjFilename = filename;
	prjFilename.replace(QRegExp("\\.stl$"), ".prj");
	if (QFile::exists(prjFilename)) {
		// read and get EPSG code
		QFile f(prjFilename);
		f.open(QFile::ReadOnly);
		auto wkt = f.readAll().toStdString();
		int epsgCode = GdalUtil::wkt2Epsg(wkt.c_str());
		if (epsgCode != 0) {
			auto cs = csBuilder->system(QString("EPSG:%1").arg(epsgCode));
			dialog.setCoordinateSystem(cs);
		} else {
			dialog.setCoordinateSystem(projectCs);
		}
	} else {
		dialog.setCoordinateSystem(projectCs);
	}

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {
		return false;
	}
	auto cs = dialog.coordinateSystem();

	if (projectCs != cs) {
		m_converter = new CoordinateSystemConverter(cs, projectCs);
	}
	return true;
}

bool GeoDataPointmapSTLImporter::importData(GeoData* data, int /*index*/, QWidget* /*w*/)
{
	GeoDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<GeoDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkSTLReader* reader = vtkSTLReader::New();
	reader->SetFileName(iRIC::toStr(filename()).c_str());
	vtkPolyData* polydata = reader->GetOutput();
	reader->Update();

	pmap->vtkGrid()->Reset();
	pmap->vtkValues()->Reset();

	vtkPoints* points = pmap->vtkGrid()->GetPoints();
	vtkDoubleArray* values = vtkDoubleArray::New();
	vtkIdType i, numpoints;
	numpoints = polydata->GetNumberOfPoints();
	points->Allocate(numpoints);
	values->Allocate(numpoints);
	for (i = 0; i < numpoints; ++i) {
		double tmpvec[3];
		polydata->GetPoints()->GetPoint(i, tmpvec);

		values->InsertNextValue(tmpvec[2]);
		QPointF p(tmpvec[0], tmpvec[1]);

		if (m_converter != nullptr) {
			p = m_converter->convert(p);
		}

		points->InsertNextPoint(p.x(), p.y(), 0);
	}
	points->Modified();
	values->Modified();

	pmap->setSTLData(polydata, values);

	reader->Delete();
	values->Delete();
	return true;
}

const QStringList GeoDataPointmapSTLImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Stereolithography file (*.stl)");
	return ret;
}

const QStringList GeoDataPointmapSTLImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "stl";
	return ret;
}

bool GeoDataPointmapSTLImporter::checkHeader(const QString& filename, QWidget* w)
{
	QFile file(filename);

	// whether ASCII or Binary
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename));
		return false;
	}
	bool ascii = true;
	char c;
	for (int i = 0; i < 256 && !file.atEnd(); i++) {
		file.getChar(&c);
		if (c == 0) {
			ascii = false;
			break;
		}
	}
	file.close();

	// check validation
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename));
		return false;
	}
	if (ascii) {
		// ASCII
		QTextStream ts(&file);
		QString theader;
		ts >> theader;
		if (ts.status() == QTextStream::Ok && theader == tr("solid")) { return true; }
	} else {
		// Binary
		QDataStream ds(&file);
		ds.setByteOrder(QDataStream::LittleEndian);  // assumed to be little endian.
		const int len = 80;
		char dummy[len];
		ds.readRawData(dummy, len);
		int num;
		const int intsize = 4;
		ds.readRawData((char*)&num, intsize);
		const int headerBytes = len + intsize;
		const int triangleBytes = 50;
		int fsize = file.size();
		if (fsize == headerBytes + triangleBytes * num) { return true; }
	}

	return false;
}
