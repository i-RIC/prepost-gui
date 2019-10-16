#ifndef GEODATARIVERSURVEYIMPORTER_PROBLEMSDIALOG_H
#define GEODATARIVERSURVEYIMPORTER_PROBLEMSDIALOG_H

#include "../geodatariversurveyimporter.h"

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataRiverSurveyImporter_ProblemsDialog;
}

class GeoDataRiverSurveyImporter::ProblemsDialog : public QDialog
{
	Q_OBJECT

public:
	struct Problem {
		QString name;
		QString problem;
	};

	explicit ProblemsDialog(QWidget *parent = nullptr);
	~ProblemsDialog();

	void setProblems(const std::vector<Problem>& problems);

private:
	Ui::GeoDataRiverSurveyImporter_ProblemsDialog *ui;
};

#endif // GEODATARIVERSURVEYIMPORTER_PROBLEMSDIALOG_H
