#include "polylineshapeexporter.h"

#include <misc/standarderrormessages.h>
#include <misc/stringtool.h>

#include <QPointF>
#include <QTextCodec>
#include <QVector>

#include <shapefil.h>

namespace {

SHPObject* getShpObject(const QVector<QPointF>& data, SHPHandle shph)
{
	int nVertices = data.size();
	std::vector<double> padfX, padfY;
	for (int i = 0; i < data.size(); ++i) {
		padfX.push_back(data.at(i).x());
		padfY.push_back(data.at(i).y());
	}

	SHPObject* ret = SHPCreateSimpleObject(SHPT_ARC, nVertices, padfX.data(), padfY.data(), NULL);
	SHPComputeExtents(ret);
	SHPRewindObject(shph, ret);

	return ret;
}
void writeDbf(DBFHandle dbfh)
{
	// Add Nave attribute
	DBFAddField(dbfh, "Name", FTString, 10, 0);
	QTextCodec* codec = QTextCodec::codecForLocale();
	DBFWriteStringAttribute(dbfh, 0, 0, codec->fromUnicode(QString("Polyline1")).data());
}

} // namespace

void PolylineShapeExporter::exportData(const QVector<QPointF>& data, const QString& filename, const QString& /*selectedFilter*/, QWidget* /*w*/)
{
	std::string fname = iRIC::toStr(filename);

	SHPHandle shph = SHPCreate(fname.c_str(), SHPT_ARC);
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

QStringList PolylineShapeExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Shape file (*.shp)");

	return ret;
}
