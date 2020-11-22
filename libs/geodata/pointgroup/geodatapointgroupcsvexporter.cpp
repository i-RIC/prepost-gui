#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "geodatapointgroupcsvexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>

GeoDataPointGroupCsvExporter::GeoDataPointGroupCsvExporter(GeoDataCreator* creator) :
	GeoDataExporter {tr("CSV file"), creator}
{}

bool GeoDataPointGroupCsvExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* pd)
{
	auto group = dynamic_cast<GeoDataPointGroup*>(data);
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
		stream << "x,y,name" << endl;
	} else {
		stream << "x,y,name,value" << endl;
	}

	auto o = pd->mainfile()->offset();
	// write data
	for (auto d : group->allData()) {
		auto p = dynamic_cast<GeoDataPointGroupPoint*> (d);
		const auto point = p->point();
		if (isRef) {
			stream << point.x() + o.x() << "," << point.y() + o.y() << "," << p->name() << endl;
		} else {
			stream << point.x() + o.x() << "," << point.y() + o.y() << "," << p->name() << "," << p->value().toDouble() << endl;
		}
	}
	file.close();

	return true;
}

const QStringList GeoDataPointGroupCsvExporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("CSV file (*.csv)");

	return ret;
}
