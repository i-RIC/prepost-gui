#ifndef GEODATARIVERSURVEYJMKEXPORTER_PROBLEMSDIALOG_H
#define GEODATARIVERSURVEYJMKEXPORTER_PROBLEMSDIALOG_H

#include "../geodatariversurveyjmkexporter.h"

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataRiverSurveyJmkExporter_ProblemsDialog;
}

class GeoDataRiverSurveyJmkExporter::ProblemsDialog : public QDialog
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

	Ui::GeoDataRiverSurveyJmkExporter_ProblemsDialog *ui;
};

#endif // GEODATARIVERSURVEYJMKEXPORTER_PROBLEMSDIALOG_H
