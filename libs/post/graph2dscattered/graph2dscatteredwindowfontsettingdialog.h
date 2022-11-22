#ifndef GRAPH2DSCATTEREDWINDOWFONTSETTINGDIALOG_H
#define GRAPH2DSCATTEREDWINDOWFONTSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class Graph2dScatteredWindowFontSettingDialog;
}

class Graph2dScatteredWindowFontSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Graph2dScatteredWindowFontSettingDialog(QWidget *parent = nullptr);
	~Graph2dScatteredWindowFontSettingDialog();

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
	Ui::Graph2dScatteredWindowFontSettingDialog *ui;
};

#endif // GRAPH2DSCATTEREDWINDOWFONTSETTINGDIALOG_H
