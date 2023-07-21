#include "postzonedatashapeexporter.h"
#include "../postzonedatacontainer.h"
#include "misc/filesystemfunction.h"
#include "misc/stringtool.h"
#include "project/projectdata.h"
#include "project/projectmainfile.h"

#include <cs/coordinatesystem.h>

#include <QFileInfo>
#include <QTextStream>

#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkExtractGrid.h>
#include <vtkUnstructuredGrid.h>

#include <shapefil.h>
#include <ogr_spatialref.h>

namespace {

void getTmpFileNames(const QString& tmp, QString* shp, QString* shx, QString* dbf)
{
	*shp = tmp;
	shp->append(".shp");
	*shx = tmp;
	shx->append(".shx");
	*dbf = tmp;
	dbf->append(".dbf");
}

void getFileNames(const QString& shp, QString* shx, QString* dbf, QString* prj)
{
	QFileInfo shpInfo(shp);

	*shx = QString("%1/%2.shx").arg(shpInfo.absolutePath()).arg(shpInfo.baseName());
	*dbf = QString("%1/%2.dbf").arg(shpInfo.absolutePath()).arg(shpInfo.baseName());
	*prj = QString("%1/%2.prj").arg(shpInfo.absolutePath()).arg(shpInfo.baseName());
}

/// export *.shp, *.shx
bool exportShp(const QString& shpFile, vtkPointSet* ps)
{
	SHPHandle shph = SHPCreate(iRIC::toStr(shpFile).c_str(), SHPT_POINT);
	if (shph == NULL) {return false;}

	for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id) {
		double v[3];
		ps->GetPoint(id, v);
		SHPObject* obj = SHPCreateSimpleObject(SHPT_POINT, 1, &(v[0]), &(v[1]), NULL);
		SHPWriteObject(shph, -1, obj);
		SHPDestroyObject(obj);
	}
	SHPClose(shph);
	return true;
}

bool exportDbf(const QString& dbfFile, vtkPointSet* ps)
{
	DBFHandle dbfh = DBFCreate(iRIC::toStr(dbfFile).c_str());
	if (dbfh == NULL) {return false;}

	// add attributes
	vtkPointData* pData = ps->GetPointData();
	int na = pData->GetNumberOfArrays();
	for (int i = 0; i < na; ++i){
		vtkAbstractArray* tmpa = pData->GetArray(i);
		if (tmpa == nullptr) {
			continue;
		}
		vtkDoubleArray* doubleA = vtkDoubleArray::SafeDownCast(tmpa);
		vtkIntArray* intA = vtkIntArray::SafeDownCast(tmpa);
		QString name = tmpa->GetName();
		DBFFieldType ftype;
		int valWidth;
		int valDecimals;

		if (doubleA != nullptr) {
			// double.
			ftype = FTDouble;
			valWidth = 40;
			valDecimals = 6;
		} else if (intA != nullptr) {
			// int.
			ftype = FTInteger;
			valWidth = 10;
			valDecimals = 0;
		}
		QStringList postFixes;
		if (tmpa->GetNumberOfComponents() == 1) {
			postFixes << "";
		} else if (tmpa->GetNumberOfComponents() == 2) {
			postFixes << "X" << "Y";
		} else if (tmpa->GetNumberOfComponents() == 3) {
			postFixes << "X" << "Y" << "Z";
		}
		for (const QString& postfix : postFixes) {
			QString tmpname = name;
			tmpname.append(postfix);
			DBFAddField(dbfh, iRIC::toStr(tmpname).c_str(), ftype, valWidth, 6);
		}
	}
	// write data.
	int fieldnum = 0;
	for (int i = 0; i < na; ++i){
		vtkAbstractArray* tmpa = pData->GetArray(i);
		if (tmpa == nullptr) {
			continue;
		}
		vtkDoubleArray* doubleA = vtkDoubleArray::SafeDownCast(tmpa);
		vtkIntArray* intA = vtkIntArray::SafeDownCast(tmpa);
		if (doubleA != nullptr) {
			// double.
			for (int j = 0; j < doubleA->GetNumberOfComponents(); ++j){
				for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id) {
					double v = doubleA->GetComponent(id, j);
					DBFWriteDoubleAttribute(dbfh, static_cast<int>(id), fieldnum, v);
				}
				++ fieldnum;
			}
		} else if (intA != nullptr) {
			// int.
			for (int j = 0; j < intA->GetNumberOfComponents(); ++j){
				for (vtkIdType id = 0; id < ps->GetNumberOfPoints(); ++id) {
					int v = intA->GetComponent(id, j);
					DBFWriteIntegerAttribute(dbfh, static_cast<int>(id), fieldnum, v);
				}
				++ fieldnum;
			}
		}
	}
	DBFClose(dbfh);
	return true;
}

bool exportPointSet(const QString& tmpFile, vtkPointSet* ps)
{
	QString shp, shx, dbf;
	getTmpFileNames(tmpFile, &shp, &shx, &dbf);

	bool ok;
	ok = exportShp(shp, ps);
	if (! ok) {return false;}
	ok = exportDbf(dbf, ps);
	if (! ok) {return false;}

	return true;
}

bool exportStructuredGrid(const QString& tmpFile, vtkStructuredGrid* sgrid, int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{
	vtkSmartPointer<vtkExtractGrid> extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetVOI(imin, imax, jmin, jmax, kmin, kmax);
	extract->SetInputData(sgrid);
	extract->Update();
	vtkStructuredGrid* egrid = extract->GetOutput();

	return exportPointSet(tmpFile, egrid);
}

bool exportPrj(const QString& prj, ProjectData* pd)
{
	CoordinateSystem* cs = pd->mainfile()->coordinateSystem();
	if (cs == nullptr) {return true;}
	OGRSpatialReference sr;
	char* wkt;
	sr.importFromProj4(iRIC::toStr(cs->proj4PlaneStr()).c_str());
	sr.exportToWkt(&wkt);
	QString WKT = wkt;
	OGRFree(wkt);
	QFile prjFile(prj);
	bool ok = prjFile.open(QFile::WriteOnly);
	if (! ok) {return false;}
	QTextStream ts(&prjFile);
	ts << WKT;
	prjFile.close();
	return true;
}

bool moveFile(const QString& from, const QString& to)
{
	QFile::remove(to);
	return QFile::rename(from, to);
}

void removeTmpFiles(const QString& shp, const QString& shx, const QString& dbf)
{
	QFile::remove(shp);
	QFile::remove(shx);
	QFile::remove(dbf);
}

bool moveFiles(const QString& tmpFile, const QString& shp)
{
	bool ok;
	QString tmpShp, tmpShx, tmpDbf;
	QString shx, dbf, prj;
	getTmpFileNames(tmpFile, &tmpShp, &tmpShx, &tmpDbf);
	getFileNames(shp, &shx, &dbf, &prj);

	ok = moveFile(tmpShp, shp);
	if (! ok) {removeTmpFiles(tmpShp, tmpShx, tmpDbf); return false;}
	ok = moveFile(tmpShx, shx);
	if (! ok) {removeTmpFiles(tmpShp, tmpShx, tmpDbf); return false;}
	ok = moveFile(tmpDbf, dbf);
	if (! ok) {removeTmpFiles(tmpShp, tmpShx, tmpDbf); return false;}
	return true;
}

}

PostZoneDataShapeExporter::PostZoneDataShapeExporter(const QString& workDir) :
	m_workDir {workDir}
{}

QString PostZoneDataShapeExporter::filename(const QString& prefix, int index) const
{
	QString fname = prefix;
	fname.append(QString("%1.shp").arg(index));
	return fname;
}

bool PostZoneDataShapeExporter::exportToFile(PostZoneDataContainer* data, const QString& shp, double /*time*/, int imin, int imax, int jmin, int jmax, int kmin, int kmax, ProjectData* pd, const QPointF& offset) const
{
	std::string tmpFile = iRIC::toStr(iRIC::getTempFileName(m_workDir));

	vtkPointSet* ps = data->data()->data();
	vtkStructuredGrid* sgrid = vtkStructuredGrid::SafeDownCast(ps);
	bool ok;
	if (sgrid != 0) {
		vtkSmartPointer<vtkStructuredGrid> copy;
		vtkStructuredGrid* grid = applyOffset<vtkStructuredGrid>(sgrid, copy, offset);
		ok = exportStructuredGrid(tmpFile.c_str(), grid, imin, imax, jmin, jmax, kmin, kmax);

	} else {
		vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(ps);
		vtkSmartPointer<vtkUnstructuredGrid> copy;
		vtkUnstructuredGrid* grid = applyOffset<vtkUnstructuredGrid>(ugrid, copy, offset);
		ok = exportPointSet(tmpFile.c_str(), grid);
	}
	if (! ok) {return false;}

	QString shx, dbf, prj;
	getFileNames(shp, &shx, &dbf, &prj);

	ok = exportPrj(prj, pd);
	if (! ok) {return false;}

	return moveFiles(tmpFile.c_str(), shp);
}
