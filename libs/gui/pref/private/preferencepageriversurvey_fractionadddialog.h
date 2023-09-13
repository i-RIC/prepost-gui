#ifndef PREFERENCEPAGERIVERSURVEY_FRACTIONADDDIALOG_H
#define PREFERENCEPAGERIVERSURVEY_FRACTIONADDDIALOG_H

#include <QDialog>

#include "../preferencepageriversurvey.h"

namespace Ui {
class PreferencePageRiverSurvey_FractionAddDialog;
}

class PreferencePageRiverSurvey::FractionAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FractionAddDialog(QWidget *parent = 0);
	~FractionAddDialog();

	double fraction() const;

private:
	Ui::PreferencePageRiverSurvey_FractionAddDialog *ui;
};

#endif // PREFERENCEPAGERIVERSURVEY_FRACTIONADDDIALOG_H
