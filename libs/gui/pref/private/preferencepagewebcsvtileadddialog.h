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

private:
	Ui::PreferencePageWebCsvTileAddDialog *ui;
};

#endif // PREFERENCEPAGEWEBCSVTILEADDDIALOG_H
