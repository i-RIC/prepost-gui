#include "geodatariverpathpoint.h"
#include "geodatariverpathpointjmkdata.h"
#include "geodatariversurvey.h"
#include "geodatariversurveyjmkexporter.h"
#include "private/geodatariversurveyjmkexporter_problemsdialog.h"

namespace {

const double OVERWRAP_MIN = 0.01;

} // namespace

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

bool GeoDataRiverSurveyJmkExporter::check(GeoDataRiverSurvey* rs, QWidget* w)
{
	std::vector<GeoDataRiverPathPoint*> points;
	auto point = rs->headPoint()->nextPoint();
	while (point != nullptr) {
		points.push_back(point);
		point = point->nextPoint();
	}

	std::vector<Problem> problems;
	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		const auto point = *it;
		const auto& jmk = point->jmk();
		for (int i = 0; i < static_cast<int> (jmk.items().size()) - 1; ++i) {
			const auto item1 = jmk.items().at(i);
			const auto item2 = jmk.items().at(i + 1);
			if (item1.distance + item1.width > item2.distance + OVERWRAP_MIN) {
				Problem p {point->name(), tr("Overwrap found between item %1 and %2.").arg(i + 1).arg(i + 2)};
				problems.push_back(p);
			}
		}
	}
	if (problems.size() == 0) {return true;}

	ProblemsDialog dialog(w);
	dialog.setProblems(problems);
	int ret = dialog.exec();

	return ret == QDialog::Accepted;
}
