#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupshpexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <misc/stringtool.h>

#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>

#include <shapefil.h>

#include <QTextCodec>

GeoDataPointGroupShpExporter::GeoDataPointGroupShpExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("ESRI Shapefile"), creator}
{}

bool GeoDataPointGroupShpExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPointGroup*>(data);
	group->mergeEditTargetPoint();

	SHPHandle shph = getSHPHandle(filename);

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	bool valueIsDouble;
	DBFHandle dbfh = getDBFHandle(dbfFilename, data->gridAttribute(), &valueIsDouble);

	auto offset = pd->mainfile()->offset();
	QTextCodec* codec = QTextCodec::codecForLocale();

	int idx = 0;
	for (auto p : group->allPoints()) {
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

const QStringList GeoDataPointGroupShpExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shapefile (*.shp)");
	return ret;
}

SHPHandle GeoDataPointGroupShpExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);
	return shph;
}

DBFHandle GeoDataPointGroupShpExporter::getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble)
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

SHPObject* GeoDataPointGroupShpExporter::getSHPObject(GeoDataPointGroupPoint* point, SHPHandle shp, int index, double xoffset, double yoffset)
{
	int nParts = 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	std::vector<double> xlist, ylist;

	QPointF p = point->point();

	partStart[0] = 0;
	nVertices = 1;
	xlist.push_back(p.x());
	ylist.push_back(p.y());

	SHPObject* ret = SHPCreateObject(SHPT_ARC, index, nParts, partStart.data(), NULL, nVertices, xlist.data(), ylist.data(), NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	return ret;
}

void GeoDataPointGroupShpExporter::outputAttributes(GeoDataPointGroupPoint* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polygon->name()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polygon->value().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polygon->value().toInt());
	}
}
