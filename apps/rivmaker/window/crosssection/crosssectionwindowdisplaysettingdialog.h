#ifndef CROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H
#define CROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H

#include "crosssectionwindowdisplaysetting.h"
#include "../../widgets/chartgraphicsviewdisplaysetting.h"

#include <QDialog>

namespace Ui {
class CrossSectionWindowDisplaySettingDialog;
}

class CrossSectionWindowDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CrossSectionWindowDisplaySettingDialog(QWidget *parent = 0);
	~CrossSectionWindowDisplaySettingDialog();

	CrossSectionWindowDisplaySetting crossSectionWindowDisplaySetting() const;
	ChartGraphicsViewDisplaySetting chartGraphicsViewDisplaySetting() const;
	void setSettings(const CrossSectionWindowDisplaySetting& csds, const ChartGraphicsViewDisplaySetting& chartds);

private slots:
	void editScalesFont();
	void editAspectRatioFont();
	void editHWMsFont();

private:
	CrossSectionWindowDisplaySetting m_crossSectionWindowDisplaySetting;
	ChartGraphicsViewDisplaySetting m_chartGraphicsViewDisplaySetting;

	Ui::CrossSectionWindowDisplaySettingDialog *ui;
};

#endif // CROSSSECTIONWINDOWDISPLAYSETTINGDIALOG_H
