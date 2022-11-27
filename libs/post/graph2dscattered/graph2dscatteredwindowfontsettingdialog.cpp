#include "graph2dscatteredwindowfontsettingdialog.h"
#include "ui_graph2dscatteredwindowfontsettingdialog.h"

Graph2dScatteredWindowFontSettingDialog::Graph2dScatteredWindowFontSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Graph2dScatteredWindowFontSettingDialog)
{
	ui->setupUi(this);
}

Graph2dScatteredWindowFontSettingDialog::~Graph2dScatteredWindowFontSettingDialog()
{
	delete ui;
}

QFont Graph2dScatteredWindowFontSettingDialog::chartTitleFont() const
{
	return ui->chartTitleWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setChartTitleFont(const QFont& font)
{
	ui->chartTitleWidget->setFont(font);
}

QFont Graph2dScatteredWindowFontSettingDialog::legendsFont() const
{
	return ui->legendWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setLegendsFont(const QFont& font)
{
	ui->legendWidget->setFont(font);
}

QFont Graph2dScatteredWindowFontSettingDialog::xAxisTitleFont() const
{
	return ui->xAxisTitleWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setXAxisTitleFont(const QFont& font)
{
	ui->xAxisTitleWidget->setFont(font);
}

QFont Graph2dScatteredWindowFontSettingDialog::xAxisTickFont() const
{
	return ui->xAxisTickWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setXAxisTickFont(const QFont& font)
{
	ui->xAxisTickWidget->setFont(font);
}

QFont Graph2dScatteredWindowFontSettingDialog::yAxisTitleFont() const
{
	return ui->yAxisTitleWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setYAxisTitleFont(const QFont& font)
{
	ui->yAxisTitleWidget->setFont(font);
}

QFont Graph2dScatteredWindowFontSettingDialog::yAxisTickFont() const
{
	return ui->yAxisTickWidget->font();
}

void Graph2dScatteredWindowFontSettingDialog::setYAxisTickFont(const QFont& font)
{
	ui->yAxisTickWidget->setFont(font);
}
