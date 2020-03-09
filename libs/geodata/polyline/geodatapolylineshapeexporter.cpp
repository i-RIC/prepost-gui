#include "geodatapolyline.h"
#include "geodatapolylineimplpolyline.h"
#include "geodatapolylineshapeexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridattributeinteger.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <misc/stringtool.h>

#include <QTextCodec>

#include <vector>

GeoDataPolyLineShapeExporter::GeoDataPolyLineShapeExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("ESRI Shapefile"), creator}
{}

SHPObject* GeoDataPolyLineShapeExporter::getSHPObject(GeoDataPolyLine* polyline, SHPHandle shp, int index, double xoffset, double yoffset)
{
	int nParts = 1;
	std::vector<int> partStart (nParts);
	int nVertices = 0;
	QList<double> xlist, ylist;

	// output region first.
	auto polyLine = polyline->polyLine()->polyLine();
	partStart[0] = 0;
	nVertices += polyLine.size();
	for (QPointF p : polyLine) {
		xlist.append(p.x() + xoffset);
		ylist.append(p.y() + yoffset);
	}
	std::vector<double> padfX(nVertices), padfY(nVertices);
	for (int i = 0; i < xlist.count(); ++i) {
		padfX[i] = xlist.at(i);
		padfY[i] = ylist.at(i);
	}

	SHPObject* ret = SHPCreateObject(SHPT_ARC, index, nParts, partStart.data(), NULL, nVertices, padfX.data(), padfY.data(), NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	return ret;
}

void GeoDataPolyLineShapeExporter::outputAttributes(GeoDataPolyLine* polyline, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polyline->caption()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polyline->variantValue().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polyline->variantValue().toInt());
	}
}

bool GeoDataPolyLineShapeExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	GeoDataPolyLine* pol = dynamic_cast<GeoDataPolyLine*>(data);

	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_ARC);

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

SHPHandle GeoDataPolyLineShapeExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_ARC);
	return shph;
}

DBFHandle GeoDataPolyLineShapeExporter::getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble)
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

const QStringList GeoDataPolyLineShapeExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shape file (*.shp)");
	return ret;
}
