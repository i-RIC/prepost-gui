#include "geodatapolylinegroup.h"
#include "geodatapolylinegrouppolyline.h"
#include "geodatapolylinegroupcsvexporter.h"

#include <cs/coordinatesystem.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

GeoDataPolyLineGroupCsvExporter::GeoDataPolyLineGroupCsvExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("CSV file"), creator}
{}

bool GeoDataPolyLineGroupCsvExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPolyLineGroup*>(data);
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
		stream << "lid,vid,x,y,name" << endl;
	} else {
		stream << "lid,vid,x,y,name,value" << endl;
	}

	auto o = pd->mainfile()->offset();
	// write data
	int lid = 1;
	for (auto d : group->allData()) {
		auto l = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (d);
		auto points = l->points();
		int vid = 1;
		for (auto point : points) {
			if (isRef) {
				stream << lid << "," << vid << "," << point.x() + o.x() << "," << point.y() + o.y() << "," << l->name() << endl;
			} else {
				stream << lid << "," << vid << "," << point.x() + o.x() << "," << point.y() + o.y() << "," << l->name() << "," << l->value().toDouble() << endl;
			}
			++ vid;
		}
		++ lid;
	}
	file.close();

	// export *.prj if project coordinate system is defined
	auto cs = pd->mainfile()->coordinateSystem();
	if (cs != nullptr) {
			auto prjFilename = filename;
			prjFilename.replace(".csv", ".prj");
			cs->exportPlaneWkt(prjFilename);
	}

	return true;
}

const QStringList GeoDataPolyLineGroupCsvExporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}
