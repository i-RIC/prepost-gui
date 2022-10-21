#ifndef GRAPH2DHYBRIDSETTINGDIALOG_H
#define GRAPH2DHYBRIDSETTINGDIALOG_H

#include <QDialog>
#include "graph2dhybridwindowresultsetting.h"

namespace Ui
{
	class Graph2dHybridSettingDialog;
}

class Graph2dHybridSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dHybridSettingDialog(QWidget* parent = nullptr);
	~Graph2dHybridSettingDialog();

	const Graph2dHybridWindowResultSetting& setting() const;
	void setSetting(const Graph2dHybridWindowResultSetting& setting);

public slots:
	void xAxisSettingUpdated();
	void yAxisComboBoxChange(int index);
	void accept() override;

private:
	bool axisRangesCheck();
	void applyXAxisSetting();
	void applyYAxisLeftSetting();
	void applyYAxisRightSetting();
	void saveYAxisLeftSetting();
	void saveYAxisRightSetting();

	Ui::Graph2dHybridSettingDialog* ui;
	Graph2dHybridWindowResultSetting m_setting;
};

#endif // GRAPH2DHYBRIDSETTINGDIALOG_H
