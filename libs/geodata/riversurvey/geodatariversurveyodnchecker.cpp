#include "geodatariversurveyodnchecker.h"
#include "geodatariversurvey.h"

#include "private/geodatariversurveyodnchecker_problemsdialog.h"

#include <vector>

bool GeoDataRiverSurveyOdnChecker::check(GeoDataRiverSurvey* rs, QWidget* w, bool forceFix)
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
	dialog.setForceFix(forceFix);
	int ret = dialog.exec();

	if (forceFix) {return false;}

	return ret == QDialog::Accepted;
}

GeoDataRiverSurveyOdnChecker::GeoDataRiverSurveyOdnChecker()
{}

