#ifndef PREFERENCEPAGEWEBCSVTILEADDDIALOG_H
#define PREFERENCEPAGEWEBCSVTILEADDDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencePageWebCsvTileAddDialog;
}

class GeoDataPointmapWebImporterSetting;

class PreferencePageWebCsvTileAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferencePageWebCsvTileAddDialog(QWidget *parent = 0);
	~PreferencePageWebCsvTileAddDialog();

	GeoDataPointmapWebImporterSetting setting() const;

	void setName(const QString& name);
	void setMinZoom(int zoom);
	void setMaxZoom(int zoom);
	void setUrl(const QString& url);

private:
	Ui::PreferencePageWebCsvTileAddDialog *ui;
};

#endif // PREFERENCEPAGEWEBCSVTILEADDDIALOG_H
