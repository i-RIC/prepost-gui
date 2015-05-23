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
	explicit XYAxisDisplaySettingDialog(QWidget* parent = nullptr);
	~XYAxisDisplaySettingDialog();
	/// The setting for X-axis
	const Setting xAxisSetting() const;
	/// Set the setting for X-axis
	void setXAxisSetting(const Setting& xSetting);
	/// The setting for Y-axis
	const Setting yAxisSetting() const;
	/// Set the setting for Y-axis
	void setYAxisSetting(const Setting& ySetting);

private:
	Ui::XYAxisDisplaySettingDialog* ui;
};

#endif // XYAXISDISPLAYSETTINGDIALOG_H
