#include "geodatapolylinegroup.h"
#include "geodatapolylinegrouppolyline.h"
#include "geodatapolylinegroupshpimporter.h"
#include "private/geodatapolylinegroup_impl.h"

#include <misc/stringtool.h>

#include <QDir>
#include <QMessageBox>
#include <QRectF>
#include <QTextCodec>
#include <QVariant>

#include <shapefil.h>

#include <geos/util/GEOSException.h>

namespace {

const double LOCATION_DELTA = 1.0E-6;

std::vector<QPointF> readPolyLine(SHPObject* shpo)
{
	std::vector<QPointF> ret;

	int start = *(shpo->panPartStart);
	int end = shpo->nVertices;
	if (shpo->nParts != 1) {
		end = *(shpo->panPartStart + 1);
	}
	QPointF lastPoint;
	for (int i = start; i < end; ++i) {
		QPointF point(*(shpo->padfX + i), *(shpo->padfY + i));
		QPointF diff = lastPoint - point;
		bool veryNear = (std::fabs(diff.x()) < LOCATION_DELTA && std::fabs(diff.y()) < LOCATION_DELTA);
		if (i != start && veryNear) {continue;}
		ret.push_back(point);
		lastPoint = point;
	}
	return ret;
}

} // namespace

GeoDataPolyLineGroupShpImporter::GeoDataPolyLineGroupShpImporter(GeoDataCreator* creator) :
	GeoDataPolyDataGroupShpImporter {"esrishape_polyline_group", tr("ESRI Shapefile (Polylines)"), creator}
{}

const QStringList GeoDataPolyLineGroupShpImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("ESRI Shapefile (Lines) (*.shp)"));
	return ret;
}

bool GeoDataPolyLineGroupShpImporter::importData(GeoData* data, int /*index*/, QWidget* w)
{
	auto group = dynamic_cast<GeoDataPolyLineGroup*>(data);

	std::string fname = iRIC::toStr(filename());
	SHPHandle shph = SHPOpen(fname.c_str(), "rb");

	QString dbfFilename = filename();
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	std::string dbfname = iRIC::toStr(dbfFilename);
	DBFHandle dbfh = DBFOpen(dbfname.c_str(), "rb");

	int type;
	double minbound[4], maxbound[4];
	int count;

	SHPGetInfo(shph, &count, &type, minbound, maxbound);

	QString nameTpl = tr("Polyline%1");
	for (int i = 0; i < count; ++i) {
		SHPObject* shpo = SHPReadObject(shph, i);
		auto ls = readPolyLine(shpo);

		auto poly = new GeoDataPolyLineGroupPolyLine(ls, group);
		// name
		QString name = nameTpl.arg(i + 1);
		if (m_nameSetting == GeoDataPolyDataGroupShpImporterSettingDialog::nsLoadFromDBF) {
			name = readData(dbfh, i, m_nameAttribute, m_codec).toString();
		}
		poly->setName(name);
		// value
		QVariant value = m_specifiedValue;
		if (m_valueSetting == GeoDataPolyDataGroupShpImporterSettingDialog::vsLoadFromDBF) {
			value = readData(dbfh, i, m_valueAttribute, m_codec);
		}
		poly->setValue(value);
		group->addData(poly);

		SHPDestroyObject(shpo);
	}
	SHPClose(shph);
	DBFClose(dbfh);

	group->setupDataItem();
	group->updateAttributeBrowser(true);
	group->updateVtkObjects();
	group->updateIndex();

	return true;
}
