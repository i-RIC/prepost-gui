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

	void setCaption(const QString& caption);
	void setUrl(const QString& url);
	void setMaxZoom(int zoom);

private:
	Ui::PreferencePageTmsAddDialog *ui;
};

#endif // PREFERENCEPAGETMSADDDIALOG_H
