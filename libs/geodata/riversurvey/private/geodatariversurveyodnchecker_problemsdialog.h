#ifndef GEODATARIVERSURVEYODNCHECKER_PROBLEMSDIALOG_H
#define GEODATARIVERSURVEYODNCHECKER_PROBLEMSDIALOG_H

#include "../geodatariversurveyodnchecker.h"

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataRiverSurveyOdnChecker_ProblemsDialog;
}

class GeoDataRiverSurveyOdnChecker::ProblemsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProblemsDialog(QWidget *parent = nullptr);
	~ProblemsDialog();

	void setProblems(const std::vector<Problem>& problems);
	void setForceFix(bool forceFix);

private slots:
	void copyToClipboard();

private:
	std::vector<Problem> m_problems;

	Ui::GeoDataRiverSurveyOdnChecker_ProblemsDialog *ui;
};

#endif // GEODATARIVERSURVEYODNCHECKER_PROBLEMSDIALOG_H
