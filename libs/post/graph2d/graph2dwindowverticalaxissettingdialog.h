#ifndef GRAPH2DWINDOWVERTICALAXISSETTINGDIALOG_H
#define GRAPH2DWINDOWVERTICALAXISSETTINGDIALOG_H

#include <QDialog>
#include "graph2dwindowverticalaxissetting.h"

namespace Ui
{
	class Graph2dWindowVerticalAxisSettingDialog;
}

class Graph2dWindowVerticalAxisSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dWindowVerticalAxisSettingDialog(QWidget* parent = nullptr);
	~Graph2dWindowVerticalAxisSettingDialog();
	void setSetting(const Graph2dWindowVerticalAxisSetting& setting);
	void setAutoRange(double min, double max);
	const Graph2dWindowVerticalAxisSetting& setting() {
		return m_setting;
	}
public slots:
	void accept() override;
private:
	Ui::Graph2dWindowVerticalAxisSettingDialog* ui;
	double m_autoRangeMin;
	double m_autoRangeMax;
	Graph2dWindowVerticalAxisSetting m_setting;
};

#endif // GRAPH2DWINDOWVERTICALAXISSETTINGDIALOG_H
