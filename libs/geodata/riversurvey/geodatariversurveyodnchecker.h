#ifndef GEODATARIVERSURVEYODNCHECKER_H
#define GEODATARIVERSURVEYODNCHECKER_H

#include <QObject>

class GeoDataRiverSurvey;

class GeoDataRiverSurveyOdnChecker : public QObject
{
	Q_OBJECT

public:
	static bool check(GeoDataRiverSurvey* rs, QWidget* w, bool forceFix);

private:
	GeoDataRiverSurveyOdnChecker();

	struct Problem {
		QString name;
		QString message;
	};

	class ProblemsDialog;
};

#endif // GEODATARIVERSURVEYODNCHECKER_H
