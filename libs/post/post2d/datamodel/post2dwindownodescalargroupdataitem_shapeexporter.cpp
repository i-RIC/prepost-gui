#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem_shapeexporter.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <cs/coordinatesystem.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>
#include <guibase/geometry/pointring.h>
#include <guibase/geometry/polygonregion.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <vtkCell.h>
#include <vtkGeometryFilter.h>
#include <vtkPointData.h>

#include <ogr_spatialref.h>
#include <shapefil.h>

#include <memory>
#include <vector>

// namespace for local classes and functions
namespace {

/// Define temporary file names
void defineTempFiles(const QString& tmpFile, QString* shpFile, QString* shxFile, QString* dbfFile)
{
	*shpFile = tmpFile;
	shpFile->append(".shp");
	*shxFile = tmpFile;
	shxFile->append(".shx");
	*dbfFile = tmpFile;
	dbfFile->append(".dbf");
}

std::vector<PolygonRegion*> setupMergedPolygons(std::vector<PolygonRegion*> pols)
{
	bool merged = false;

	for (int i = 0; i < pols.size(); ++i) {
		PolygonRegion* r1 = pols.at(i);
		for (int j = i + 1; j < pols.size(); ++j) {
			PolygonRegion* r2 = pols.at(j);
			if (r1->merge(r2)) {
				merged = true;
				delete r2;
				pols.erase(pols.begin() + j);
				-- j;
			}
		}
	}
	if (! merged) {
		for (PolygonRegion* r : pols) {
			r->clean();
		}
		return pols;
	}

	return setupMergedPolygons(pols);
}


std::vector<PolygonRegion*> setupMergedPolygons(vtkPolyData* polyData)
{
	vtkIdType numCells = polyData->GetNumberOfCells();
	std::vector<PolygonRegion*> ret;

	for (vtkIdType cellId = 0; cellId < numCells; ++cellId) {
		vtkCell* cell = polyData->GetCell(cellId);
		vtkIdType numPoints = cell->GetNumberOfPoints();
		if (numPoints < 3) {continue;}
		PolygonRegion* cellRegion = new PolygonRegion(cell, polyData->GetPoints());
		bool merged = false;
		for (PolygonRegion* region : ret) {
			merged = region->merge(cellRegion);
			if (merged) {break;}
		}
		if (merged) {
			delete cellRegion;
		} else {
			ret.push_back(cellRegion);
		}
	}
	return setupMergedPolygons(ret);
}

/// Move file. If the target file existed, it will be overwritten.
bool moveFile(const QString& from, const QString &to)
{
	QFile::remove(to);
	return QFile::rename(from, to);
}

/// Rename files from temporary files to output files
bool renameFiles(const QString& shpFileName, const QString &tmpShpFileName)
{
	QFileInfo shpFileInfo(shpFileName);
	QString shxFileName = QString("%1/%2.shx").arg(shpFileInfo.absolutePath()).arg(shpFileInfo.baseName());
	QString dbfFileName = QString("%1/%2.dbf").arg(shpFileInfo.absolutePath()).arg(shpFileInfo.baseName());

	QFileInfo tmpShpFileInfo(tmpShpFileName);
	QString tmpShxFileName = QString("%1/%2.shx").arg(tmpShpFileInfo.absolutePath()).arg(tmpShpFileInfo.baseName());
	QString tmpDbfFileName = QString("%1/%2.dbf").arg(tmpShpFileInfo.absolutePath()).arg(tmpShpFileInfo.baseName());

	if (! moveFile(tmpShpFileName, shpFileName)) {
		goto ERROR;
	}
	if (! moveFile(tmpShxFileName, shxFileName)) {
		goto ERROR;
	}
	if (! moveFile(tmpDbfFileName, dbfFileName)) {
		goto ERROR;
	}
	return true;

ERROR:
	QFile::remove(tmpShpFileName);
	QFile::remove(tmpShxFileName);
	QFile::remove(tmpDbfFileName);

	return false;
}

bool outputPrjFile(CoordinateSystem* cs, const QString& shpFileName)
{
	if (cs == nullptr) {return true;}

	QFileInfo shpFileInfo(shpFileName);
	QString prjFileName = QString("%1/%2.prj").arg(shpFileInfo.absolutePath()).arg(shpFileInfo.baseName());
	OGRSpatialReference sr;
	char* wkt;

	sr.importFromProj4(iRIC::toStr(cs->proj4PlaneStr()).c_str());
	sr.exportToWkt(&wkt);
	QString WKT = wkt;
	OGRFree(wkt);

	QFile prjFile(prjFileName);
	bool ok = prjFile.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QTextStream ts(&prjFile);
	ts << WKT;
	prjFile.close();
	return true;
}

void outputPolygon(PolygonRegion* region, SHPHandle shph, DBFHandle dbfh, double min, double max, int polygonId)
{
	int nParts = static_cast<int>(region->holes().size()) + 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	std::vector<double> xvec, yvec;

	// output region.
	PointRing* regionRing = region->region();
	partStart[0] = 0;
	const std::vector<vtkIdType>& points = regionRing->points();
	nVertices += (static_cast<int>(points.size()) + 1);
	double v[3];
	for (int i = 0; i < points.size() + 1; ++i) {
		vtkIdType id = points.at(i % points.size());
		regionRing->getVtkPoints()->GetPoint(id, v);
		xvec.push_back(v[0]);
		yvec.push_back(v[1]);
	}
	for (int i = 0; i < region->holes().size(); ++i) {
		PointRing* holeRing = region->holes().at(i);
		partStart[i + 1] = nVertices;
		const std::vector<vtkIdType>& holePoints = holeRing->points();
		nVertices += (static_cast<int>(holePoints.size()) + 1);
		for (int i = 0; i < holePoints.size() + 1; ++i) {
			vtkIdType id = holePoints.at(i % holePoints.size());
			holeRing->getVtkPoints()->GetPoint(id, v);
			xvec.push_back(v[0]);
			yvec.push_back(v[1]);
		}
	}

	SHPObject* obj = SHPCreateObject(SHPT_POLYGON, polygonId, nParts, partStart.data(), NULL, nVertices, xvec.data(), yvec.data(), NULL, NULL);
	SHPComputeExtents(obj);
	SHPRewindObject(shph, obj);
	SHPWriteObject(shph, -1, obj);
	SHPDestroyObject(obj);

	QString valueStr;
	if (min == - HUGE_VAL && max == HUGE_VAL) {
		valueStr = "All range";
	} else if (min == - HUGE_VAL) {
		valueStr = QString("- %1").arg(max);
	} else if (max == HUGE_VAL) {
		valueStr = QString("%1 -").arg(min);
	} else {
		valueStr = QString("%1 - %2").arg(min).arg(max);
	}
	DBFWriteStringAttribute(dbfh, polygonId, 0, iRIC::toStr(valueStr).c_str());
	DBFWriteDoubleAttribute(dbfh, polygonId, 1, min);
	DBFWriteDoubleAttribute(dbfh, polygonId, 2, max);
}


void outputPointRing(const PointRing& ring, std::ostream& os)
{
	for (vtkIdType id : ring.points()) {
		os << id << " ";
	}
	os << endl;
}

void outputRegion(const PolygonRegion& region, std::ostream& os)
{
	os << "Region: " << endl;
	outputPointRing(*region.region(), os);
	os << "Holes: " << endl;
	for (auto h : region.holes()) {
		outputPointRing(*h, os);
	}
}

} // namespace

Post2dWindowNodeScalarGroupDataItem::ShapeExporter::ShapeExporter(Post2dWindowNodeScalarGroupDataItem* parent) :
	m_parent {parent}
{}

bool Post2dWindowNodeScalarGroupDataItem::ShapeExporter::exportContourFigure(const QString& filename, double /*time*/)
{
	// define a temporary file names.
	QString shpFile, shxFile, dbfFile;
	defineTempFiles(m_parent->projectData()->tmpFileName(), &shpFile, &shxFile, &dbfFile);

	// open  *.shp and *.shx file.
	SHPHandle shph = SHPCreate(iRIC::toStr(shpFile).c_str(), SHPT_POLYGON);
	if (shph == NULL) {return false;}
	// open *.dbf file.
	DBFHandle dbfh = DBFCreate(iRIC::toStr(dbfFile).c_str());
	if (dbfh == NULL) {return false;}
	DBFAddField(dbfh, "Value", FTString, 255, 0);
	DBFAddField(dbfh, "ValueMin", FTDouble, 40, 6);
	DBFAddField(dbfh, "ValueMax", FTDouble, 40, 6);

	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_parent->parent()->parent());
	LookupTableContainer* stc = typedi->nodeLookupTable(m_parent->m_setting.target);
	if (stc == nullptr) {return false;}
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	// setup value-clipped poly data from the original data.
	PostZoneDataContainer* c = dynamic_cast<Post2dWindowZoneDataItem*>(m_parent->parent())->dataContainer();

	vtkSmartPointer<vtkGeometryFilter> geom = vtkSmartPointer<vtkGeometryFilter>::New();
	geom->SetInputData(c->data());
	geom->Update();

	vtkPolyData* rcp = m_parent->createRangeClippedPolyData(geom->GetOutput());
	vtkPolyData* inputPolyData = m_parent->createValueClippedPolyData(rcp);
	inputPolyData->GetPointData()->SetActiveScalars(iRIC::toStr(m_parent->m_setting.target).c_str());

	rcp->Delete();

	double delta = (range[1] - range[0]) / static_cast<double>(m_parent->m_setting.numberOfDivisions);
	int polygonId = 0;

	for (int i = 0; i < m_parent->m_setting.numberOfDivisions; i++) {

		double lowValue = range[0] + static_cast<double>(i) * delta;
		double highValue = range[0] + static_cast<double>(i + 1) * delta;

		if (i == 0) {
			lowValue = - HUGE_VAL;
		}
		if (i == m_parent->m_setting.numberOfDivisions - 1){
			highValue = HUGE_VAL;
		}

		vtkPolyData* lowClipped = setupLowerClippedPolygon(inputPolyData, lowValue);
		vtkPolyData* bothClipped = setupHigherClippedPolygon(lowClipped, highValue);

		std::vector<PolygonRegion*> regions = setupMergedPolygons(bothClipped);
		for (PolygonRegion* r : regions) {
			outputPolygon(r, shph, dbfh, lowValue, highValue, polygonId);
			++ polygonId;
			delete r;
		}
		bothClipped->Delete();

		inputPolyData->Delete();
		inputPolyData = lowClipped;
		inputPolyData->Register(0);
	}
	inputPolyData->UnRegister(0);

	SHPClose(shph);
	DBFClose(dbfh);

	// rename files
	if (! renameFiles(filename, shpFile)) {
		return false;
	}

	// output *.prj if needed
	if (! outputPrjFile(m_parent->projectData()->mainfile()->coordinateSystem(), filename)) {
		return false;
	}
	return true;
}
