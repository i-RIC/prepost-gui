#ifndef GRAPH2DHYBRIDWINDOWFONTSETTINGDIALOG_H
#define GRAPH2DHYBRIDWINDOWFONTSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class Graph2dHybridWindowFontSettingDialog;
}

class Graph2dHybridWindowFontSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dHybridWindowFontSettingDialog(QWidget *parent = nullptr);
	~Graph2dHybridWindowFontSettingDialog();

	QFont chartTitleFont() const;
	void setChartTitleFont(const QFont& font);

	QFont legendsFont() const;
	void setLegendsFont(const QFont& font);

	QFont xAxisTitleFont() const;
	void setXAxisTitleFont(const QFont& font);

	QFont xAxisTickFont() const;
	void setXAxisTickFont(const QFont& font);

	QFont yAxisTitleFont() const;
	void setYAxisTitleFont(const QFont& font);

	QFont yAxisTickFont() const;
	void setYAxisTickFont(const QFont& font);

private:
	Ui::Graph2dHybridWindowFontSettingDialog *ui;
};

#endif // GRAPH2DHYBRIDWINDOWFONTSETTINGDIALOG_H
