#include "postzonedatavtkexporter.h"
#include "../postzonedatacontainer.h"
#include "misc/filesystemfunction.h"
#include "misc/stringtool.h"

#include <QFile>
#include <QVector2D>

#include <vtkExtractGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridWriter.h>

PostZoneDataVtkExporter::PostZoneDataVtkExporter(const QString& workDir) :
	PostZoneDataVtkExporter {workDir, Mode::ASCII}
{}

PostZoneDataVtkExporter::PostZoneDataVtkExporter(const QString &workDir, Mode mode) :
	m_workDir {workDir},
	m_mode {mode}
{}

QString PostZoneDataVtkExporter::filename(const QString& prefix, int index) const
{
	QString fname = prefix;
	fname.append(QString("%1.vtk").arg(index));
	return fname;
}

bool PostZoneDataVtkExporter::exportToFile(PostZoneDataContainer* data, const QString& filename, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData*, const QPointF& offset) const
{
	std::string tmpFile = iRIC::toStr(iRIC::getTempFileName(m_workDir));
	auto ps = data->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(ps);
	auto ugrid = vtkUnstructuredGrid::SafeDownCast(ps);
	if (sgrid != 0) {
		vtkSmartPointer<vtkStructuredGrid> copy;
		vtkStructuredGrid* grid = applyOffset<vtkStructuredGrid>(sgrid, copy, offset);
		exportStructuredGrid(tmpFile.c_str(), grid, time, imin, imax, jmin, jmax, kmin, kmax);
	} else if (ugrid != 0) {
		vtkSmartPointer<vtkUnstructuredGrid> copy;
		vtkUnstructuredGrid* grid = applyOffset<vtkUnstructuredGrid>(ugrid, copy, offset);
		exportUnstructuredGrid(tmpFile.c_str(), grid, time);
	}
	if (QFile::exists(filename)) {
		bool ok = QFile::remove(filename);
		if (! ok){
			QFile::remove(tmpFile.c_str());
			return false;
		}
	}
	bool ok = QFile::rename(tmpFile.c_str(), filename);
	if (! ok){
		QFile::remove(tmpFile.c_str());
		return false;
	}
	return true;
}

void PostZoneDataVtkExporter::exportStructuredGrid(const char* filename, vtkStructuredGrid* sgrid, double time, int imin, int imax, int jmin, int jmax, int kmin, int kmax) const
{
	vtkExtractGrid* extract = vtkExtractGrid::New();
	extract->SetVOI(imin, imax, jmin, jmax, kmin, kmax);
	extract->SetInputData(sgrid);
	vtkStructuredGridWriter* writer = vtkStructuredGridWriter::New();
	setupWriter(writer, filename, time);
	writer->SetInputConnection(extract->GetOutputPort());
	writer->Update();

	extract->Delete();
	writer->Delete();
}

void PostZoneDataVtkExporter::exportUnstructuredGrid(const char* filename, vtkUnstructuredGrid* ugrid, double time) const
{
	vtkUnstructuredGridWriter* writer = vtkUnstructuredGridWriter::New();
	setupWriter(writer, filename, time);
	writer->SetInputData(ugrid);
	writer->Update();

	writer->Delete();
}

void PostZoneDataVtkExporter::setupWriter(vtkDataWriter *writer, const char* filename, double time) const
{
	QString header("iRIC output t = %1");
	writer->SetHeader(iRIC::toStr(header.arg(time)).c_str());
	writer->SetFileName(filename);
	if (m_mode == Mode::ASCII) {
		writer->SetFileTypeToASCII();
	} else if (m_mode == Mode::BINARY) {
		writer->SetFileTypeToBinary();
	}
}
