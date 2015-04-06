#include "rawdatapointmapstlexporter.h"
#include "rawdatapointmapt.h"
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <misc/stringtool.h>

#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSTLWriter.h>

RawDataPointmapSTLExporter::RawDataPointmapSTLExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("Stereolithography (STL)");
}

bool RawDataPointmapSTLExporter::doEmport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* /*w*/, ProjectData* pd)
{
	RawDataPointMapT<double, vtkDoubleArray>* pmap = dynamic_cast<RawDataPointMapT<double, vtkDoubleArray>*>(data);
	vtkPolyData* polydata = vtkPolyData::New();
	polydata->DeepCopy(pmap->delaunayedPolyData());

	double tmpvec[3];
	vtkIdType i, numpoints;
	numpoints = polydata->GetNumberOfPoints();
	vtkDoubleArray* val = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("values"));
	QVector2D offset = pd->mainfile()->offset();
	for (i = 0; i < numpoints; ++i){
		polydata->GetPoints()->GetPoint(i, tmpvec);
		tmpvec[0] += offset.x();
		tmpvec[1] += offset.y();
		tmpvec[2] = val->GetValue(i);
		polydata->GetPoints()->SetPoint(i, tmpvec);
	}
	vtkSTLWriter* writer = vtkSTLWriter::New();
	if (selectedFilter == tr("Binary STL (*.stl)")){
		writer->SetFileTypeToBinary();
	}else{
		writer->SetFileTypeToASCII();
	}
	writer->SetFileName(iRIC::toStr(filename).c_str());
	writer->SetInputData(polydata);
	writer->Write();

	writer->Delete();
	polydata->Delete();
	return true;
}

const QStringList RawDataPointmapSTLExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ASCII STL (*.stl)");
	ret << tr("Binary STL (*.stl)");
	return ret;
}
