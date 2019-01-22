#ifndef PREFERENCEPAGERIVERSURVEYFRACTIONADDDIALOG_H
#define PREFERENCEPAGERIVERSURVEYFRACTIONADDDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencePageRiverSurveyFractionAddDialog;
}

class PreferencePageRiverSurveyFractionAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferencePageRiverSurveyFractionAddDialog(QWidget *parent = 0);
	~PreferencePageRiverSurveyFractionAddDialog();

	double fraction() const;

private:
	Ui::PreferencePageRiverSurveyFractionAddDialog *ui;
};

#endif // PREFERENCEPAGERIVERSURVEYFRACTIONADDDIALOG_H
