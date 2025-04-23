#include "../../base/iricmainwindowi.h"
#include "../../grid/v4grid.h"
#include "../../grid/v4structured2dgrid.h"
#include "../../grid/v4structured3dgrid.h"
#include "../../grid/v4unstructured2dgrid.h"
#include "../../project/projectdata.h"
#include "../postsolutioninfo.h"
#include "../v4solutiongrid.h"
#include "../v4postzonedatacontainer.h"
#include "postzonedatatpoexporter.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtktool/vtkpointsutil.h>
#include <misc/informationdialog.h>

#include <QFile>
#include <QTextStream>
#include <QVector2D>

#include <vtkPointData.h>

namespace {

void exportPointDataToStream(QTextStream& stream, vtkPointSet* ps, vtkDataArray* da, vtkIdType index, int component, const QPointF& offset)
{
	double v[3];
	ps->GetPoints()->GetPoint(index, v);
	double* tuple = da->GetTuple(index);

	stream << v[0] + offset.x() << "," << v[1] + offset.y() << "," << *(tuple + component) << "\r\n";
}

void exportCellDataToStream(QTextStream& stream, vtkPointSet* ps, vtkDataArray* da, vtkIdType index, const QPointF& offset, int component = 0)
{
	auto cell = ps->GetCell(index);
	auto center = vtkPointsUtil::getCenter(cell);
	double* tuple = da->GetTuple(index);

	stream << center.x() + offset.x() << "," << center.y() + offset.y() << "," << *(tuple + component) << "\r\n";
}

void exportStructured2dGrid(QTextStream& stream, v4Structured2dGrid* grid, int arrayid, int component,
														int imin, int imax, int jmin, int jmax, int kmin, int kmax, const QPointF& offset)
{
	vtkDataArray* dataArray = grid->vtkData()->data()->GetPointData()->GetArray(arrayid);
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				vtkIdType index = grid->pointIndex(i, j);
				exportPointDataToStream(stream, grid->vtkData()->data(), dataArray, index, component, offset);
			}
		}
	}
}

void exportStructured3dGrid(QTextStream& stream, v4Structured3dGrid* grid, int arrayid, int component,
														int imin, int imax, int jmin, int jmax, int kmin, int kmax, const QPointF& offset)
{
	vtkDataArray* dataArray = grid->vtkData()->data()->GetPointData()->GetArray(arrayid);
	for (int k = kmin; k <= kmax; ++k){
		for (int j = jmin; j <= jmax; ++j){
			for (int i = imin; i <= imax; ++i){
				vtkIdType index = grid->pointIndex(i, j, k);
				exportPointDataToStream(stream, grid->vtkData()->data(), dataArray, index, component, offset);
			}
		}
	}
}

void exportUnstructuredGrid(QTextStream& stream, v4Unstructured2dGrid* grid, int arrayid, int component, const QPointF& offset)
{
	vtkDataArray* dataArray = grid->vtkData()->data()->GetPointData()->GetArray(arrayid);
	for (vtkIdType index = 0; index < grid->nodeCount(); ++index) {
		exportPointDataToStream(stream, grid->vtkData()->data(), dataArray, index, component, offset);
	}
}

bool exportComoponentFile(v4Grid* grid, const QString& filename, const QString& componentName, int arrayid, int component, int imin, int imax, int jmin, int jmax, int kmin, int kmax, const QPointF& offset)
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

	auto sgrid2d = dynamic_cast<v4Structured2dGrid*> (grid);
	auto sgrid3d = dynamic_cast<v4Structured3dGrid*> (grid);
	auto ugrid = dynamic_cast<v4Unstructured2dGrid*> (grid);

	if (sgrid2d != nullptr) {
		exportStructured2dGrid(stream, sgrid2d, arrayid, component, imin, imax, jmin, jmax, kmin, kmax, offset);
	} else if (sgrid3d != nullptr) {
		exportStructured3dGrid(stream, sgrid3d, arrayid, component, imin, imax, jmin, jmax, kmin, kmax, offset);
	} else if (ugrid != nullptr) {
		exportUnstructuredGrid(stream, ugrid, arrayid, component, offset);
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

bool PostZoneDataTpoExporter::exportToFile(v4PostZoneDataContainer* c, const QString& filename, double /*time*/, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* projectData, const QPointF& offset) const
{
	QString componentName;

	auto grid = c->gridData()->grid();

	// currently calculation result defined at grid nodes are output
	auto pd = grid->vtkData()->data()->GetPointData();

	bool ok;
	for (int i = 0; i < pd->GetNumberOfArrays(); ++i){
		vtkDataArray* array = pd->GetArray(i);
		if (array == nullptr){continue;}

		QString name = array->GetName();
		int comps = array->GetNumberOfComponents();
		if (comps == 1){
			componentName = name;
			ok = exportComoponentFile(grid, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}
		} else if (comps == 2){
			componentName = name + "X";
			ok = exportComoponentFile(grid, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}

			componentName = name + "Y";
			ok = exportComoponentFile(grid, filename, componentName, i, 1, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}
		} else if (comps == 3){
			componentName = name + "X";
			ok = exportComoponentFile(grid, filename, componentName, i, 0, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}

			componentName = name + "Y";
			ok = exportComoponentFile(grid, filename, componentName, i, 1, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}

			componentName = name + "Z";
			ok = exportComoponentFile(grid, filename, componentName, i, 2, imin, imax, jmin, jmax, kmin, kmax, offset);
			if (! ok){return false;}
		}
	}

	if (m_isFirstStep) {
		InformationDialog::warning(projectData->mainWindow(), PostSolutionInfo::tr("Warning"),
															 PostSolutionInfo::tr("tpo files export calculation result defined at grid nodes."),
															 "postzonedatatpoexporter_warning");
	}

	return true;
}
