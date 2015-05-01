#ifndef GRAPH2DHYBRIDWINDOWLINESETTINGDIALOG_H
#define GRAPH2DHYBRIDWINDOWLINESETTINGDIALOG_H

#include <QDialog>

#include "graph2dhybridwindowresultsetting.h"

namespace Ui
{
	class Graph2dHybridWindowLineSettingDialog;
}

class Graph2dHybridWindowLineSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindowLineSettingDialog(QWidget* parent = nullptr);
	~Graph2dHybridWindowLineSettingDialog();
	void setLineWidth(int width);
	int lineWidth();
	void setCustomColor(QColor col);
	QColor customColor();
	void setAxisSide(Graph2dHybridWindowResultSetting::AxisSide side);
	Graph2dHybridWindowResultSetting::AxisSide axisSide();

private:
	Ui::Graph2dHybridWindowLineSettingDialog* ui;
};

#endif // GRAPH2DHYBRIDWINDOWLINESETTINGDIALOG_H
