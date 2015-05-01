#ifndef GRAPH2DSCATTEREDAXISSETTINGDIALOG_H
#define GRAPH2DSCATTEREDAXISSETTINGDIALOG_H

#include <QDialog>
#include "graph2dscatteredwindowresultsetting.h"

namespace Ui
{
	class Graph2dScatteredAxisSettingDialog;
}

class Graph2dScatteredAxisSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dScatteredAxisSettingDialog(QWidget* parent = nullptr);
	~Graph2dScatteredAxisSettingDialog();
	void setSetting(const Graph2dScatteredWindowResultSetting& setting);
	const Graph2dScatteredWindowResultSetting& setting() {return m_setting;}

public slots:
	void xAxisRadioButtonToggled();
	void yAxisComboBoxChange(int index);
	void accept();

private:
	bool axisRangesCheck();
	void applyXAxisSetting();
	void applyYAxisLeftSetting();
	void applyYAxisRightSetting();
	void saveYAxisLeftSetting();
	void saveYAxisRightSetting();

	Ui::Graph2dScatteredAxisSettingDialog* ui;
	Graph2dScatteredWindowResultSetting m_setting;
};

#endif // GRAPH2DSCATTEREDAXISSETTINGDIALOG_H
