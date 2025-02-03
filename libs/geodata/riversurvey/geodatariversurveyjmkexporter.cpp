#include "geodatariverpathpoint.h"
#include "geodatariverpathpointjmkdata.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyjmkexporter.h"

GeoDataRiverSurveyJmkExporter::GeoDataRiverSurveyJmkExporter() :
	QObject {}
{}

bool GeoDataRiverSurveyJmkExporter::doExport(const QString& filename, GeoDataRiverSurvey* rs, QWidget* w)
{
	std::vector<GeoDataRiverPathPoint*> points;

	auto point = rs->headPoint()->nextPoint();
	while (point != nullptr) {
		points.push_back(point);
		point = point->nextPoint();
	}

	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("Cannot open file for writing"));
		return false;
	}

	QTextStream outstream(&file);
	int lineNo = 1;
	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		auto point = *it;
		outstream << QString("%1").arg(lineNo, 5);
		outstream << QString("%1").arg(point->name(), 8);
		outstream << QString("%1").arg(point->jmk().items().size(), 5) << "\n";
		for (const auto& item : point->jmk().items()) {
			outstream << QString("%1").arg(item.distance, 8, 'f', 2);
			outstream << QString("%1").arg(item.width, 8, 'f', 2);
			outstream << QString("%1").arg(item.height, 8, 'f', 2);
			outstream << QString("%1").arg(item.submerged, 5);
			outstream << QString("%1").arg(item.dense, 5);
			outstream << QString("%1").arg(item.dead, 5);
			outstream << QString("%1").arg(item.highLow, 5);
			outstream << QString("%1").arg(item.lowBranchHeight, 8, 'f', 2) << "\n";
		}
		++ lineNo;
	}
	outstream << "E" << "\n";
}
