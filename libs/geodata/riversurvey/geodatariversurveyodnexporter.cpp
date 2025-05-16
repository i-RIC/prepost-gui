#include "geodatariversurvey.h"
#include "geodatariversurveyodnexporter.h"
#include "private/geodatariversurveyodnexporter_problemsdialog.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationitem.h>

#include <vector>

GeoDataRiverSurveyOdnExporter::GeoDataRiverSurveyOdnExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Cross-Section Data (*.odn)"), creator)
{}

bool GeoDataRiverSurveyOdnExporter::doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd)
{
	auto rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	auto ok = check(rs, w);
	if (! ok) {return false;}

	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		std::cerr << "Cannot open file for writing: "
							<< qPrintable(file.errorString()) << std::endl;
		return false;
	}

	QTextStream outstream(&file);

	auto offset = pd->mainfile()->offset();

	std::vector<GeoDataRiverPathPoint*> points;
	auto tmpp = rs->headPoint()->nextPoint();
	while (true) {
		points.push_back(tmpp);
		tmpp = tmpp->nextPoint();
		if (tmpp == nullptr) {break;}
	}

	for (auto it = points.rbegin(); it != points.rend(); ++it) {
		auto point = *it;
		outstream << QString("%1").arg(point->odn().nb(2) + 1, 5);
		outstream << QString("%1").arg(point->odn().nb(3) + 1, 5);
		outstream << QString("%1").arg(point->crosssection().AltitudeInfo().size(), 5);
		outstream << QString("%1").arg(point->name(), 10);
		outstream << QString("%1").arg(point->odn().spanDistance(), 10, 'f', 2);
		double hwl = 0;
		auto item = rs->defaultWSE()->getItem(point->name());
		if (item != nullptr) {
			hwl = item->value();
		}
		outstream << QString("%1").arg(hwl, 10, 'f', 2);
		outstream << QString("%1").arg(point->odn().nb(1) + 1, 5);
		outstream << QString("%1").arg(point->odn().nb(4) + 1, 5);
		outstream << QString("%1").arg(point->odn().nb(0) + 1, 5);
		outstream << QString("%1").arg(point->odn().nb(5) + 1, 5);
		outstream << "\n";

		const auto& cs = point->crosssection();
		const auto& al = cs.AltitudeInfo();
		for (int i = 0; i < al.size(); ++i) {
			auto a = al.at(i);
			outstream << QString("%1").arg(a.position() + cs.leftShift(), 8, 'f', 2);
			outstream << QString("%1").arg(a.height(), 8, 'f', 2);
			if ((i + 1) % 5 == 0) {
				outstream << "\n";
			}
		}
		if (al.size() % 5 != 0) {
			outstream << "\n";
		}
	}

	file.close();

	return true;

}
const QStringList GeoDataRiverSurveyOdnExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("Cross-Section Data (*.odn)");
	return ret;
}

bool GeoDataRiverSurveyOdnExporter::check(GeoDataRiverSurvey* rs, QWidget* w)
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
		const auto& odn = point->odn();
		int nb[6];
		for (int i = 0; i < 6; ++i) {
			nb[i] = odn.nb(i);
		}
		bool all_ok = true;
		for (int i = 0; i < 5; ++i) {
			const auto& nb1 = nb[i];
			const auto& nb2 = nb[i + 1];
			if (nb2 < nb1) {
				all_ok = false;
			}
		}
		if (! all_ok) {
			Problem p {point->name(), tr("ODN data points are not ordered correctly.")};
			problems.push_back(p);
		}
	}

	if (problems.size() == 0) {return true;}

	ProblemsDialog dialog(w);
	dialog.setProblems(problems);
	int ret = dialog.exec();

	return ret == QDialog::Accepted;
}
