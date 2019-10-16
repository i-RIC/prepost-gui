#ifndef GEODATARIVERSURVEYMLITIMPORTER_PROBLEMSDIALOG_H
#define GEODATARIVERSURVEYMLITIMPORTER_PROBLEMSDIALOG_H

#include "../geodatariversurveymlitimporter.h"

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataRiverSurveyMlitImporter_ProblemsDialog;
}

class GeoDataRiverSurveyMlitImporter::ProblemsDialog : public QDialog
{
	Q_OBJECT

public:
	struct Problem {
		QString name;
		QString relatedFile;
		QString problem;
	};

	explicit ProblemsDialog(QWidget *parent = nullptr);
	~ProblemsDialog();

	void setProblems(const std::vector<Problem>& problems);

private:
	Ui::GeoDataRiverSurveyMlitImporter_ProblemsDialog *ui;
};

#endif // GEODATARIVERSURVEYMLITIMPORTER_PROBLEMSDIALOG_H
