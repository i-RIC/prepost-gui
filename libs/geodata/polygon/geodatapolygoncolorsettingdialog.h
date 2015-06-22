#ifndef GEODATAPOLYGONCOLORSETTINGDIALOG_H
#define GEODATAPOLYGONCOLORSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataPolygonColorSettingDialog;
}

class GeoDataPolygonColorSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum Mapping {Value, Arbitrary};
	GeoDataPolygonColorSettingDialog(QWidget* parent = nullptr);
	~GeoDataPolygonColorSettingDialog();
	void setMapping(Mapping m);
	void setOpacityPercent(int o);
	Mapping mapping();
	void setColor(const QColor& color);
	QColor color();
	int opacityPercent();

private:
	Ui::GeoDataPolygonColorSettingDialog* ui;
};

#endif // GEODATAPOLYGONCOLORSETTINGDIALOG_H
