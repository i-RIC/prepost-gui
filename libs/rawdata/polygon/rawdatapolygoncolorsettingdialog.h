#ifndef RAWDATAPOLYGONCOLORSETTINGDIALOG_H
#define RAWDATAPOLYGONCOLORSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataPolygonColorSettingDialog;
}

class RawDataPolygonColorSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum Mapping {Value, Arbitrary};
	RawDataPolygonColorSettingDialog(QWidget* parent = nullptr);
	~RawDataPolygonColorSettingDialog();
	void setMapping(Mapping m);
	void setOpacityPercent(int o);
	Mapping mapping();
	void setColor(const QColor& color);
	QColor color();
	int opacityPercent();

private:
	Ui::RawDataPolygonColorSettingDialog* ui;
};

#endif // RAWDATAPOLYGONCOLORSETTINGDIALOG_H
