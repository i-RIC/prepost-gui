#include "geodatapolylinegroup.h"
#include "geodatapolylinegrouppolyline.h"
#include "geodatapolylinegroupshpexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <misc/stringtool.h>

#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>

#include <shapefil.h>

#include <QTextCodec>

GeoDataPolyLineGroupShpExporter::GeoDataPolyLineGroupShpExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("ESRI Shapefile"), creator}
{}

bool GeoDataPolyLineGroupShpExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPolyLineGroup*>(data);
	group->mergeEditTargetData();

	SHPHandle shph = getSHPHandle(filename);

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	bool valueIsDouble;
	DBFHandle dbfh = getDBFHandle(dbfFilename, data->gridAttribute(), &valueIsDouble);

	auto offset = pd->mainfile()->offset();
	QTextCodec* codec = QTextCodec::codecForName("UTF-8");

	int idx = 0;
	for (auto d : group->allData()) {
		auto p = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (d);
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

const QStringList GeoDataPolyLineGroupShpExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shapefile (*.shp)");
	return ret;
}

SHPHandle GeoDataPolyLineGroupShpExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_ARC);
	return shph;
}

DBFHandle GeoDataPolyLineGroupShpExporter::getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble)
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

SHPObject* GeoDataPolyLineGroupShpExporter::getSHPObject(GeoDataPolyLineGroupPolyLine* polyLine, SHPHandle shp, int index, double xoffset, double yoffset)
{
	int nParts = 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	std::vector<double> xlist, ylist;


	geos::geom::LineString* ls = polyLine->getGeosLineString();

	partStart[0] = 0;
	nVertices += static_cast<int> (ls->getNumPoints());
	for (int i = 0; i < ls->getNumPoints(); ++i) {
		const auto& coord = ls->getCoordinateN(i);
		xlist.push_back(coord.x + xoffset);
		ylist.push_back(coord.y + yoffset);
	}

	SHPObject* ret = SHPCreateObject(SHPT_ARC, index, nParts, partStart.data(), NULL, nVertices, xlist.data(), ylist.data(), NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	return ret;
}

void GeoDataPolyLineGroupShpExporter::outputAttributes(GeoDataPolyLineGroupPolyLine* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polygon->name()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polygon->value().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polygon->value().toInt());
	}
}
