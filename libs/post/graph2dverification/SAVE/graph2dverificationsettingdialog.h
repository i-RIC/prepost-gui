#ifndef GRAPH2DVERIFICATIONSETTINGDIALOG_H
#define GRAPH2DVERIFICATIONSETTINGDIALOG_H

#include <QDialog>
#include "graph2dverificationwindowresultsetting.h"

namespace Ui
{
	class Graph2dVerificationSettingDialog;
}

class Graph2dVerificationSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dVerificationSettingDialog(QWidget* parent = nullptr);
	~Graph2dVerificationSettingDialog();
	void setSetting(const Graph2dVerificationWindowResultSetting& setting);
	const Graph2dVerificationWindowResultSetting& setting() {return m_setting;}

public slots:
	void xAxisRadioButtonToggled();
	void yAxisComboBoxChange(int index);
	void accept() override;

private:
	bool axisRangesCheck();
	void applyXAxisSetting();
	void applyYAxisLeftSetting();
	void applyYAxisRightSetting();
	void saveYAxisLeftSetting();
	void saveYAxisRightSetting();

	Ui::Graph2dVerificationSettingDialog* ui;
	Graph2dVerificationWindowResultSetting m_setting;
};

#endif // GRAPH2DVERIFICATIONSETTINGDIALOG_H
