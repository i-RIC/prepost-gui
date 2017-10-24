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

	int lineWidth() const;
	void setLineWidth(int width);

	QColor customColor() const;
	void setCustomColor(QColor col);

	Graph2dHybridWindowResultSetting::AxisSide axisSide() const;
	void setAxisSide(Graph2dHybridWindowResultSetting::AxisSide side);

private:
	Ui::Graph2dHybridWindowLineSettingDialog* ui;
};

#endif // GRAPH2DHYBRIDWINDOWLINESETTINGDIALOG_H
