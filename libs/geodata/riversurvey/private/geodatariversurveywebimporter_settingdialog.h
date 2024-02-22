#ifndef GEODATARIVERSURVEYWEBIMPORTER_SETTINGDIALOG_H
#define GEODATARIVERSURVEYWEBIMPORTER_SETTINGDIALOG_H

#include "../geodatariversurveywebimporter.h"

#include <QDialog>
#include <QStringList>


namespace Ui {
class GeoDataRiverSurveyWebImporter_SettingDialog;
}

class GeoDataRiverSurveyWebImporter::SettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingDialog(QWidget *parent = nullptr);
	~SettingDialog();

	Setting setting() const;

private slots:
	void readRiverIds();
	void readKPs();
	void readYears();
	void handleDone();

private:
	void enableOkButton();
	void disableOkButton();

	QStringList m_riverIds;
	QStringList m_KPs;
	QStringList m_years;

	QNetworkAccessManager* m_webAccessManager;
	QNetworkReply* m_webReply;

	bool m_isWaitingHttpResponse;

	Ui::GeoDataRiverSurveyWebImporter_SettingDialog *ui;
};

#endif // GEODATARIVERSURVEYWEBIMPORTER_SETTINGDIALOG_H
