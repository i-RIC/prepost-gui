#include "geodatapolygongroup.h"
#include "geodatapolygongrouppolygon.h"
#include "geodatapolygongroupshpexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <misc/stringtool.h>

#include <geos/geom/LinearRing.h>
#include <geos/geom/Polygon.h>

#include <shapefil.h>

#include <QTextCodec>

GeoDataPolygonGroupShpExporter::GeoDataPolygonGroupShpExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("ESRI Shapefile"), creator}
{}

bool GeoDataPolygonGroupShpExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPolygonGroup*>(data);
	group->mergeEditTargetPolygon();

	SHPHandle shph = getSHPHandle(filename);

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	bool valueIsDouble;
	DBFHandle dbfh = getDBFHandle(dbfFilename, data->gridAttribute(), &valueIsDouble);

	auto offset = pd->mainfile()->offset();
	QTextCodec* codec = QTextCodec::codecForLocale();

	int idx = 0;
	for (auto p : group->allPolygons()) {
		SHPObject* obj = getSHPObject(p, shph, idx, offset.x(), offset.y());
		SHPWriteObject(shph, -1, obj);
		SHPDestroyObject(obj);

		outputAttributes(p, dbfh, idx, valueIsDouble, codec);
		++ idx;
	}

	SHPClose(shph);
	DBFClose(dbfh);

	return true;
}

const QStringList GeoDataPolygonGroupShpExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shapefile (*.shp)");
	return ret;
}

SHPHandle GeoDataPolygonGroupShpExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);
	return shph;
}

DBFHandle GeoDataPolygonGroupShpExporter::getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble)
{
	DBFHandle dbfh = DBFCreate(iRIC::toStr(filename).c_str());
	// Add name and value attributes.
	DBFAddField(dbfh, "Name", FTString, 1000, 0);
	*isDouble = (dynamic_cast<SolverDefinitionGridAttributeReal*>(cond) != nullptr);
	if (*isDouble) {
		// Real
		DBFAddField(dbfh, "Value", FTDouble, 40, 6);
	} else {
		// Integer
		DBFAddField(dbfh, "Value", FTInteger, 10, 0);
	}
	return dbfh;
}

SHPObject* GeoDataPolygonGroupShpExporter::getSHPObject(GeoDataPolygonGroupPolygon* polygon, SHPHandle shp, int index, double xoffset, double yoffset)
{
	geos::geom::Polygon* pol = polygon->geosPolygon();
	int nParts = pol->getNumInteriorRing() + 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	std::vector<double> xlist, ylist;

	// region first.
	auto elr = pol->getExteriorRing();
	partStart[0] = 0;
	nVertices += elr->getNumPoints();
	for (int i = 0; i < elr->getNumPoints(); ++i) {
		const auto& coord = elr->getCoordinateN(i);
		xlist.push_back(coord.x + xoffset);
		ylist.push_back(coord.y + yoffset);
	}

	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		auto ilr = pol->getInteriorRingN(i);
		partStart[i + 1] = nVertices;
		nVertices += ilr->getNumPoints();
		for (int j = 0; j < ilr->getNumPoints(); ++j) {
			const auto& coord = ilr->getCoordinateN(j);
			xlist.push_back(coord.x + xoffset);
			ylist.push_back(coord.y + yoffset);
		}
	}

	SHPObject* ret = SHPCreateObject(SHPT_POLYGON, index, nParts, partStart.data(), NULL, nVertices, xlist.data(), ylist.data(), NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	return ret;
}

void GeoDataPolygonGroupShpExporter::outputAttributes(GeoDataPolygonGroupPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polygon->name()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polygon->value().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polygon->value().toInt());
	}
}
