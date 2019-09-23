#ifndef GRAPH2DVERIFICATIONWINDOWLINESETTINGDIALOG_H
#define GRAPH2DVERIFICATIONWINDOWLINESETTINGDIALOG_H

#include <QDialog>

#include "graph2dverificationwindowresultsetting.h"

namespace Ui
{
	class Graph2dVerificationWindowLineSettingDialog;
}

class Graph2dVerificationWindowLineSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dVerificationWindowLineSettingDialog(QWidget* parent = nullptr);
	~Graph2dVerificationWindowLineSettingDialog();

	int lineWidth() const;
	void setLineWidth(int width);

	QColor customColor() const;
	void setCustomColor(QColor col);

	Graph2dVerificationWindowResultSetting::AxisSide axisSide() const;
	void setAxisSide(Graph2dVerificationWindowResultSetting::AxisSide side);

private:
	Ui::Graph2dVerificationWindowLineSettingDialog* ui;
};

#endif // GRAPH2DVERIFICATIONWINDOWLINESETTINGDIALOG_H
