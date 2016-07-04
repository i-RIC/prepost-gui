#ifndef PREFERENCEPAGETMSADDDIALOG_H
#define PREFERENCEPAGETMSADDDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencePageTmsAddDialog;
}

class TmsImageSetting;

class PreferencePageTmsAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferencePageTmsAddDialog(QWidget *parent = 0);
	~PreferencePageTmsAddDialog();

	TmsImageSetting setting();

private:
	Ui::PreferencePageTmsAddDialog *ui;
};

#endif // PREFERENCEPAGETMSADDDIALOG_H
