#include "geodatapolygon.h"
#include "geodatapolygonholepolygon.h"
#include "geodatapolygonregionpolygon.h"
#include "geodatapolygonshapeexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <misc/stringtool.h>

#include <QTextCodec>

#include <vector>

GeoDataPolygonShapeExporter::GeoDataPolygonShapeExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("ESRI Shape file"), creator}
{}

SHPObject* GeoDataPolygonShapeExporter::getSHPObject(GeoDataPolygon* polygon, SHPHandle shp, int index, double xoffset, double yoffset)
{
	int nParts = polygon->holePolygons().size() + 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	QList<double> xlist, ylist;

	// output region first.
	QPolygonF region = polygon->regionPolygon()->polygon();
	partStart[0] = 0;
	nVertices += region.count();
	for (QPointF p : region) {
		xlist.append(p.x() + xoffset);
		ylist.append(p.y() + yoffset);
	}
	for (int i = 0; i < polygon->holePolygons().size(); ++i) {
		GeoDataPolygonHolePolygon* holepol = polygon->holePolygons().at(i);
		QPolygonF hole = holepol->polygon();
		partStart[i + 1] = nVertices;
		nVertices += hole.count();
		for (QPointF p : hole) {
			xlist.append(p.x() + xoffset);
			ylist.append(p.y() + yoffset);
		}
	}
	std::vector<double> padfX(nVertices), padfY(nVertices);
	for (int i = 0; i < xlist.count(); ++i) {
		padfX[i] = xlist.at(i);
		padfY[i] = ylist.at(i);
	}

	SHPObject* ret = SHPCreateObject(SHPT_POLYGON, index, nParts, partStart.data(), NULL, nVertices, padfX.data(), padfY.data(), NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	return ret;
}

void GeoDataPolygonShapeExporter::outputAttributes(GeoDataPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polygon->caption()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polygon->variantValue().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polygon->variantValue().toInt());
	}
}

bool GeoDataPolygonShapeExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	GeoDataPolygon* pol = dynamic_cast<GeoDataPolygon*>(data);

	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);

	auto offset = pd->mainfile()->offset();
	SHPObject* obj = getSHPObject(pol, shph, 0, offset.x(), offset.y());
	SHPWriteObject(shph, -1, obj);
	SHPClose(shph);

	SHPDestroyObject(obj);

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFCreate(dbfname.c_str());

	// Add name and value attributes.
	DBFAddField(dbfh, "Name", FTString, 1000, 0);
	SolverDefinitionGridAttribute* cond = data->gridAttribute();
	bool valueIsDouble = (dynamic_cast<SolverDefinitionGridAttributeReal*>(cond) != nullptr);
	if (valueIsDouble) {
		// Real
		DBFAddField(dbfh, "Value", FTDouble, 40, 6);
	} else {
		// Integer
		DBFAddField(dbfh, "Value", FTInteger, 10, 0);
	}
	QTextCodec* codec = QTextCodec::codecForLocale();
	if (! codec->canEncode(data->caption())) {
		codec = QTextCodec::codecForName("UTF-8");
	}
	outputAttributes(pol, dbfh, 0, valueIsDouble, codec);
	DBFClose(dbfh);

	return true;
}

SHPHandle GeoDataPolygonShapeExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);
	return shph;
}

DBFHandle GeoDataPolygonShapeExporter::getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble)
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

const QStringList GeoDataPolygonShapeExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shape file (*.shp)");
	return ret;
}
