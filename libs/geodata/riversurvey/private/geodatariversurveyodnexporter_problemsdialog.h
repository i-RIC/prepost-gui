#ifndef GEODATARIVERSURVEYODNEXPORTER_PROBLEMSDIALOG_H
#define GEODATARIVERSURVEYODNEXPORTER_PROBLEMSDIALOG_H

#include "../geodatariversurveyodnexporter.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyOdnExporter_ProblemsDialog;
}

class GeoDataRiverSurveyOdnExporter::ProblemsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProblemsDialog(QWidget *parent = nullptr);
	~ProblemsDialog();

	void setProblems(const std::vector<Problem>& problems);

private slots:
	void copyToClipboard();

private:
	std::vector<Problem> m_problems;

	Ui::GeoDataRiverSurveyOdnExporter_ProblemsDialog *ui;
};

#endif // GEODATARIVERSURVEYODNEXPORTER_PROBLEMSDIALOG_H
