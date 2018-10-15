#include "postzonedatatpoexporter.h"
#include "../postzonedatacontainer.h"

#include <QFile>
#include <QTextStream>
#include <QVector2D>

#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>

namespace {

void exportPointToStream(QTextStream& stream, vtkPoints* points, vtkDataArray* da, vtkIdType index, int component = 0)
{
	double v[3];
	points->GetPoint(index, v);
	double* tuple = da->GetTuple(index);

	stream << v[0] << "," << v[1] << "," << *(tuple + component) << "\r\n";
}

void exportStructuredGrid(PostZoneDataContainer* c, QTextStream& stream, vtkStructuredGrid* sgrid, int arrayid, int component, int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{
	vtkPoints* points = sgrid->GetPoints();
	vtkDataArray* dataArray = sgrid->GetPointData()->GetArray(arrayid);
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				vtkIdType index = c->nodeIndex(i, j, k);
				exportPointToStream(stream, points, dataArray, index, component);
			}
		}
	}
}

void exportUnstructuredGrid(QTextStream& stream, vtkUnstructuredGrid* sgrid, int arrayid, int component)
{
	vtkPoints* points = sgrid->GetPoints();
	vtkDataArray* dataArray = sgrid->GetPointData()->GetArray(arrayid);
	for (vtkIdType index = 0; index < points->GetNumberOfPoints(); ++index) {
		exportPointToStream(stream, points, dataArray, index, component);
	}
}

bool exportComoponentFile(PostZoneDataContainer* c, vtkPointSet* ps, const QString& filename, const QString& componentName, int arrayid, int component, int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{
	QString fname2 = filename;
	fname2.replace(".tpo", QString("_") + componentName + ".tpo");

	if (QFile::exists(fname2)) {
		bool ok = QFile::remove(fname2);
		if (! ok) {
			return false;
		}
	}
	QFile f(fname2);
	bool ok = f.open(QIODevice::WriteOnly);
	if (! ok) {return false;}
	QTextStream stream(&f);
	stream.setRealNumberPrecision(12);

	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(ps);

	if (sgrid != nullptr) {
		exportStructuredGrid(c, stream, sgrid, arrayid, component, imin, imax, jmin, jmax, kmin, kmax);
	} else {
		exportUnstructuredGrid(stream, ugrid, arrayid, component);
	}

	f.close();
	return true;
}

} // namespace

PostZoneDataTpoExporter::PostZoneDataTpoExporter()
{}

PostZoneDataTpoExporter::~PostZoneDataTpoExporter()
{}

QString PostZoneDataTpoExporter::filename(const QString& prefix, int index) const
{
	QString fname = prefix;
	fname.append(QString("%1.tpo").arg(index));
	return fname;
}

bool PostZoneDataTpoExporter::exportToFile(PostZoneDataContainer* c, const QString& filename, double /*time*/, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* /*projectdata*/, const QPointF& offset) const
{
	QString componentName;

	vtkSmartPointer<vtkStructuredGrid> target_sgrid;
	vtkSmartPointer<vtkUnstructuredGrid> target_ugrid;
	vtkPointSet* ps = nullptr;

	vtkStructuredGrid* source_sgrid = vtkStructuredGrid::SafeDownCast(c->data());
	vtkUnstructuredGrid* source_ugrid = vtkUnstructuredGrid::SafeDownCast(c->data());

	if (source_sgrid != nullptr) {
		ps = applyOffset<vtkStructuredGrid>(source_sgrid, target_sgrid, offset);
	} else if (source_ugrid != nullptr) {
		ps = applyOffset<vtkUnstructuredGrid>(source_ugrid, target_ugrid, offset);
	}
	vtkPointData* pd = ps->GetPointData();
	bool ok;
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pd->GetArray(i);
		if (array == nullptr){continue;}

		QString name = array->GetName();
		int comps = array->GetNumberOfComponents();
		if (comps == 1){
			componentName = name;
			ok = exportComoponentFile(c, ps, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}
		} else if (comps == 2){
			componentName = name + "X";
			ok = exportComoponentFile(c, ps, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}

			componentName = name + "Y";
			ok = exportComoponentFile(c, ps, filename, componentName, i, 1, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}
		} else if (comps == 3){
			componentName = name + "X";
			ok = exportComoponentFile(c, ps, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}

			componentName = name + "Y";
			ok = exportComoponentFile(c, ps, filename, componentName, i, 1, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}

			componentName = name + "Z";
			ok = exportComoponentFile(c, ps, filename, componentName, i, 2, imin, imax, jmin, jmax, kmin, kmax);
			if (! ok){return false;}
		}
	}
	return true;
}
