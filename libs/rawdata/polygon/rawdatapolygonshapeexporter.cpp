#include "rawdatapolygon.h"
#include "rawdatapolygonholepolygon.h"
#include "rawdatapolygonregionpolygon.h"
#include "rawdatapolygonshapeexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridrelatedconditiont.h>
#include <misc/stringtool.h>

#include <QTextCodec>

RawDataPolygonShapeExporter::RawDataPolygonShapeExporter(RawDataCreator* creator)
	: RawDataExporter(creator)
{
	m_caption = tr("ESRI Shape file");
}

SHPObject* RawDataPolygonShapeExporter::getSHPObject(RawDataPolygon* polygon, SHPHandle shp, int index, double xoffset, double yoffset)
{
	int nParts = polygon->m_holePolygons.count() + 1;
	int* partStart = new int[nParts];
	int nVertices = 0;
	double* padfX, *padfY;
	QList<double> xlist, ylist;

	// output region first.
	QPolygonF region = polygon->m_gridRegionPolygon->polygon();
	*partStart = 0;
	nVertices += region.count();
	for (QPointF p : region) {
		xlist.append(p.x() + xoffset);
		ylist.append(p.y() + yoffset);
	}
	for (int i = 0; i < polygon->m_holePolygons.count(); ++i) {
		RawDataPolygonHolePolygon* holepol = polygon->m_holePolygons.at(i);
		QPolygonF hole = holepol->polygon();
		*(partStart + i + 1) = nVertices;
		nVertices += hole.count();
		for (QPointF p : hole) {
			xlist.append(p.x() + xoffset);
			ylist.append(p.y() + yoffset);
		}
	}
	padfX = new double[nVertices];
	padfY = new double[nVertices];
	for (int i = 0; i < xlist.count(); ++i) {
		*(padfX + i) = xlist.at(i);
		*(padfY + i) = ylist.at(i);
	}

	SHPObject* ret = SHPCreateObject(SHPT_POLYGON, index, nParts, partStart, NULL, nVertices, padfX, padfY, NULL, NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shp, ret);

	delete partStart;
	delete padfX;
	delete padfY;

	return ret;
}

void RawDataPolygonShapeExporter::outputAttributes(RawDataPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec)
{
	DBFWriteStringAttribute(hDBF, index, 0, codec->fromUnicode(polygon->caption()).data());
	if (isDouble) {
		DBFWriteDoubleAttribute(hDBF, index, 1, polygon->variantValue().toDouble());
	} else {
		DBFWriteIntegerAttribute(hDBF, index, 1, polygon->variantValue().toInt());
	}
}

bool RawDataPolygonShapeExporter::doEmport(RawData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/, ProjectData* pd)
{
	RawDataPolygon* pol = dynamic_cast<RawDataPolygon*>(data);

	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);

	QVector2D offset = pd->mainfile()->offset();
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
	SolverDefinitionGridRelatedCondition* cond = data->gridRelatedCondition();
	bool valueIsDouble = (dynamic_cast<SolverDefinitionGridRelatedRealCondition*>(cond) != 0);
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

SHPHandle RawDataPolygonShapeExporter::getSHPHandle(QString filename)
{
	std::string fname = iRIC::toStr(filename);
	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);
	return shph;
}

DBFHandle RawDataPolygonShapeExporter::getDBFHandle(QString filename, SolverDefinitionGridRelatedCondition* cond, bool* isDouble)
{
	DBFHandle dbfh = DBFCreate(iRIC::toStr(filename).c_str());
	// Add name and value attributes.
	DBFAddField(dbfh, "Name", FTString, 1000, 0);
	*isDouble = (dynamic_cast<SolverDefinitionGridRelatedRealCondition*>(cond) != 0);
	if (*isDouble) {
		// Real
		DBFAddField(dbfh, "Value", FTDouble, 40, 6);
	} else {
		// Integer
		DBFAddField(dbfh, "Value", FTInteger, 10, 0);
	}
	return dbfh;
}

const QStringList RawDataPolygonShapeExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("ESRI Shape file (*.shp)");
	return ret;
}
