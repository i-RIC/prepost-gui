#include "geodatapolygongroup.h"
#include "geodatapolygongrouppolygon.h"
#include "geodatapolygongroupcsvexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/informationdialog.h>

#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

GeoDataPolygonGroupCsvExporter::GeoDataPolygonGroupCsvExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("CSV file"), creator}
{}

bool GeoDataPolygonGroupCsvExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPolygonGroup*>(data);
	group->mergeEditTargetData();

	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured while exporting to %1.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}
	QTextStream stream(&file);
	stream.setRealNumberNotation(QTextStream::FixedNotation);
	stream.setRealNumberPrecision(3);
	stream.setCodec("UTF-8");
	// write header
	bool isRef = data->gridAttribute()->isReferenceInformation();
	if (isRef) {
		stream << "pid,vid,x,y,name" << endl;
	} else {
		stream << "pid,vid,x,y,name,value" << endl;
	}

	auto o = pd->mainfile()->offset();
	// write data
	int lid = 1;
	bool holeExists = false;
	for (auto d : group->allData()) {
		auto p = dynamic_cast<GeoDataPolygonGroupPolygon*> (d);
		auto polygon = p->geosPolygon();
		auto ring = polygon->getExteriorRing();
		auto pid = 1;
		for (std::size_t i = 0; i < ring->getNumPoints(); ++i) {
			auto point = ring->getPointN(i);
			if (isRef) {
				stream << lid << "," << pid << "," << point->getX() + o.x() << "," << point->getY() + o.y() << "," << p->name() << endl;
			} else {
				stream << lid << "," << pid << "," << point->getX() + o.x() << "," << point->getY() + o.y() << "," << p->name() << "," << p->value().toDouble() << endl;
			}
			++ pid;
		}
		++ lid;

		holeExists = holeExists || (polygon->getNumInteriorRing() > 0);
	}
	file.close();

	if (holeExists) {
		InformationDialog::warning(w, tr("Warning"), tr("Some polygon in this data has holes, but holes are not exported to CSV file. If you want to export holes, please export to ESRI Shapefile."), "polygongroup_csv_export_hole");
	}

	return true;
}

const QStringList GeoDataPolygonGroupCsvExporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}
