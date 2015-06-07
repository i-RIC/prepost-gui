#include "rawdatapointmapstlimporter.h"
#include "rawdatapointmapt.h"

#include <misc/stringtool.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <vtkDoubleArray.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkVertex.h>

RawDataPointmapSTLImporter::RawDataPointmapSTLImporter(RawDataCreator* creator)
	: RawDataImporter(creator)
{
	m_caption = tr("Stereolithography (STL)");
}

bool RawDataPointmapSTLImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* /*count*/, QWidget* w)
{
	return checkHeader(filename, w);
}

bool RawDataPointmapSTLImporter::importData(RawData* data, int /*index*/, QWidget* /*w*/)
{
	RawDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<RawDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkSTLReader* reader = vtkSTLReader::New();
	reader->SetFileName(iRIC::toStr(m_filename).c_str());
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
		tmpvec[2] = 0;
		points->InsertNextPoint(tmpvec);
	}
	points->Modified();
	values->Modified();
	pmap->setPoints(points, values);
	pmap->doDelaunay();
	reader->Delete();
	values->Delete();
	return true;
}

const QStringList RawDataPointmapSTLImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Stereolithography file (*.stl)");
	return ret;
}

const QStringList RawDataPointmapSTLImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "stl";
	return ret;
}

bool RawDataPointmapSTLImporter::checkHeader(const QString& filename, QWidget* w)
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
