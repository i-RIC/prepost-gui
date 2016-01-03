#ifndef XYAXISDISPLAYSETTINGDIALOG_H
#define XYAXISDISPLAYSETTINGDIALOG_H

#include "guibase_global.h"

#include <QDialog>

namespace Ui
{
	class XYAxisDisplaySettingDialog;
}

/// Dialog to edit X and Y axis display setting
class GUIBASEDLL_EXPORT XYAxisDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	/// Setting information container
	class Setting
	{
	public:
		Setting() :
			isAuto {true}, numOfLabels {2}
		{}

		bool isAuto;
		double min;
		double max;
		int numOfLabels;
	};
	explicit XYAxisDisplaySettingDialog(QWidget* parent = nullptr);
	~XYAxisDisplaySettingDialog();

	Setting xAxisSetting() const;
	void setXAxisSetting(const Setting& xSetting);

	Setting yAxisSetting() const;
	void setYAxisSetting(const Setting& ySetting);

private:
	Ui::XYAxisDisplaySettingDialog* ui;
};

#endif // XYAXISDISPLAYSETTINGDIALOG_H
