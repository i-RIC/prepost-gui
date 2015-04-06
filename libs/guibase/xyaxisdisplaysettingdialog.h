#ifndef XYAXISDISPLAYSETTINGDIALOG_H
#define XYAXISDISPLAYSETTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>

namespace Ui {
	class XYAxisDisplaySettingDialog;
}

class GUIBASEDLL_EXPORT XYAxisDisplaySettingDialog : public QDialog
{
	Q_OBJECT
public:
	class Setting {
	public:
		bool isAuto;
		double min;
		double max;
		int numOfLabels;
		/// Constructor
		Setting() {
			isAuto = true;
			numOfLabels = 2;
		}
	};
	explicit XYAxisDisplaySettingDialog(QWidget *parent = 0);
	~XYAxisDisplaySettingDialog();
	void setXAxisSetting(const Setting& xSetting);
	const Setting xAxisSetting();
	void setYAxisSetting(const Setting& ySetting);
	const Setting yAxisSetting();

private:
	Ui::XYAxisDisplaySettingDialog *ui;
};

#endif // XYAXISDISPLAYSETTINGDIALOG_H
