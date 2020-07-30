#ifndef VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H
#define VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H

#include "../../widgets/chartgraphicsviewdisplaysetting.h"

#include <QDialog>

namespace Ui {
class VerticalCrossSectionWindowDisplaySettingDialog;
}

class VerticalCrossSectionWindowDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit VerticalCrossSectionWindowDisplaySettingDialog(QWidget *parent = nullptr);
	~VerticalCrossSectionWindowDisplaySettingDialog();

	ChartGraphicsViewDisplaySetting chartGraphicsViewDisplaySetting() const;
	void setSettings(const ChartGraphicsViewDisplaySetting& chartds);

private slots:
	void editScalesFont();
	void editAspectRatioFont();

private:
	ChartGraphicsViewDisplaySetting m_chartGraphicsViewDisplaySetting;

	Ui::VerticalCrossSectionWindowDisplaySettingDialog *ui;
};

#endif // VERTICALCROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H
