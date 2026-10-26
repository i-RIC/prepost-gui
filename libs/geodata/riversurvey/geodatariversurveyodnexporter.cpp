#include "geodatariversurvey.h"
#include "geodatariversurveyodnchecker.h"
#include "geodatariversurveyodnexporter.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevation.h>
#include <hydraulicdata/riversurveywaterelevation/hydraulicdatariversurveywaterelevationitem.h>

#include <vector>
#include <cmath>
#include <QDebug>
#include <QMessageBox>

GeoDataRiverSurveyOdnExporter::GeoDataRiverSurveyOdnExporter(GeoDataCreator* creator) :
	GeoDataExporter(tr("Cross-Section Data (*.odn)"), creator)
{}

bool GeoDataRiverSurveyOdnExporter::doExport(GeoData* data, const QString& filename, const QString& /*selectedFilter*/, QWidget* w, ProjectData* pd)
{
	auto rs = dynamic_cast<GeoDataRiverSurvey*>(data);
	auto ok = check(rs, w);
	if (! ok) {return false;}
	auto numberFits = [](double value, int width, bool separatorRequired) {
		return std::isfinite(value) && QString::number(value, 'f', 2).size() < width + (separatorRequired ? 0 : 1);
	};
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		const auto& cs = p->crosssection();
		const auto& alts = cs.AltitudeInfo();
		double hwl = 0;
		auto item = rs->defaultWSE()->getItem(p->name());
		if (item != nullptr) {hwl = item->value();}
		bool fits = p->name().size() <= 10 && ! p->name().contains('\n') && ! p->name().contains('\r') &&
			alts.size() <= 99999 && numberFits(p->odn().spanDistance(), 10, false) && numberFits(hwl, 10, false);
		for (int i = 0; i < 6; ++i) {
			const auto nb = static_cast<qint64>(p->odn().nb(i)) + 1;
			fits = fits && nb >= 0 && nb <= 99999;
		}
		for (const auto& alt : alts) {
			fits = fits && numberFits(alt.position() + cs.leftShift(), 8, true) && numberFits(alt.height(), 8, true);
		}
		if (! fits) {
			const auto message = tr("Cross section %1 cannot be exported in ODN format. A name, point count, point index or numeric value exceeds its fixed-width field, or contains a non-finite value. No file was written.").arg(p->name());
			if (w != nullptr) {
				QMessageBox::critical(w, tr("Error"), message);
			} else {
				qWarning().noquote() << message;
			}
			return false;
		}
	}
	QStringList sectionsWithInactivePoints;
	for (auto p = rs->headPoint()->nextPoint(); p != nullptr; p = p->nextPoint()) {
		if (p->crosssection().numOfAltitudes(true) != p->crosssection().numOfAltitudes(false)) {
			sectionsWithInactivePoints.push_back(p->name());
		}
	}
	if (! sectionsWithInactivePoints.isEmpty()) {
		const auto message = tr("ODN files cannot store inactive point flags. Inactive points will be exported and become active when imported again. Cross sections: %1\n\nContinue exporting?").arg(sectionsWithInactivePoints.join(", "));
		if (w == nullptr) {
			qWarning().noquote() << message;
			return false;
		}
		if (QMessageBox::warning(w, tr("Warning"), message, QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
			return false;
		}
	}

	QFile file(filename);
	if (! file.open(QIODevice::WriteOnly)) {
		std::cerr << "Cannot open file for writing: "
							<< qPrintable(file.errorString()) << std::endl;
		return false;
	}

	QTextStream outstream(&file);

	// auto offset = pd->mainfile()->offset();

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
		for (int i = 0; i < static_cast<int> (al.size()); ++i) {
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
	return GeoDataRiverSurveyOdnChecker::check(rs, w, false);
}
