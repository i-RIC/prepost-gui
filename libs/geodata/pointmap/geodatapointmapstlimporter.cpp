#include "geodatapointmapstlimporter.h"
#include "geodatapointmap.h"

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
	auto pmap = dynamic_cast<GeoDataPointmap*>(data);

	auto reader = vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(iRIC::toStr(filename()).c_str());
	reader->Update();
	vtkPolyData* polydata = reader->GetOutput();

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto values = vtkSmartPointer<vtkDoubleArray>::New();

	vtkIdType numPoints = polydata->GetNumberOfPoints();
	points->Allocate(numPoints);
	values->Allocate(numPoints);
	double tmpvec[3];
	for (vtkIdType i = 0; i < numPoints; ++i) {
		polydata->GetPoints()->GetPoint(i, tmpvec);

		QPointF p(tmpvec[0], tmpvec[1]);
		if (m_converter != nullptr) {
			p = m_converter->convert(p);
		}
		points->InsertNextPoint(p.x(), p.y(), 0);
		values->InsertNextValue(tmpvec[2]);
	}
	auto tin = vtkSmartPointer<vtkPolyData>::New();
	tin->SetPoints(points);
	tin->SetPolys(polydata->GetPolys());

	pmap->setTin(tin, values);

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
