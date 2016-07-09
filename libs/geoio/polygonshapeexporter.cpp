#include "polygonshapeexporter.h"

#include <misc/standarderrormessages.h>
#include <misc/stringtool.h>

#include <QPolygonF>
#include <QTextCodec>

#include <shapefil.h>

namespace {

SHPObject* getShpObject(const QPolygonF& data, SHPHandle shph)
{
	int nVertices = data.size();
	std::vector<double> padfX, padfY;
	for (int i = 0; i < data.size(); ++i) {
		padfX.push_back(data.at(i).x());
		padfY.push_back(data.at(i).y());
	}

	SHPObject* ret = SHPCreateSimpleObject(SHPT_POLYGON, nVertices, padfX.data(), padfY.data(), NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shph, ret);

	return ret;
}
void writeDbf(DBFHandle dbfh)
{
	// Add Nave attribute
	DBFAddField(dbfh, "Name", FTString, 10, 0);
	QTextCodec* codec = QTextCodec::codecForLocale();
	DBFWriteStringAttribute(dbfh, 0, 0, codec->fromUnicode(QString("Polygon1")).data());
}

} // namespace

void PolygonShapeExporter::exportData(const QPolygonF& data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/)
{
	std::string fname = iRIC::toStr(filename);

	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_POLYGON);
	SHPObject* obj = getShpObject(data, shph);
	SHPWriteObject(shph, -1, obj);
	SHPClose(shph);

	SHPDestroyObject(obj);

	QString dbfFilename = filename;
	dbfFilename.replace(QRegExp("shp$"), "dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);

	DBFHandle dbfh = DBFCreate(dbfname.c_str());
	writeDbf(dbfh);
	DBFClose(dbfh);
}

QStringList PolygonShapeExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Shape file (*.shp)");

	return ret;
}
