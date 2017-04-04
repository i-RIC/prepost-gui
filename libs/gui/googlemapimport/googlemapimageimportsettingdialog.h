#ifndef GOOGLEMAPIMAGEIMPORTSETTINGDIALOG_H
#define GOOGLEMAPIMAGEIMPORTSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GoogleMapImageImportSettingDialog;
}

class GoogleMapImageImportSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GoogleMapImageImportSettingDialog(QWidget* parent = nullptr);
	~GoogleMapImageImportSettingDialog();

	void setZoom(int zoom);
	void setZoomMax(int zoomMax);

	int zoom() const;
	QString maptype() const;

private:
	Ui::GoogleMapImageImportSettingDialog* ui;
};

#endif // GOOGLEMAPIMAGEIMPORTSETTINGDIALOG_H
