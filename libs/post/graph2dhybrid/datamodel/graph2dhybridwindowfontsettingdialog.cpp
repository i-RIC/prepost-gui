#include "graph2dhybridwindowfontsettingdialog.h"
#include "ui_graph2dhybridwindowfontsettingdialog.h"

Graph2dHybridWindowFontSettingDialog::Graph2dHybridWindowFontSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Graph2dHybridWindowFontSettingDialog)
{
	ui->setupUi(this);
}

Graph2dHybridWindowFontSettingDialog::~Graph2dHybridWindowFontSettingDialog()
{
	delete ui;
}

QFont Graph2dHybridWindowFontSettingDialog::chartTitleFont() const
{
	return ui->chartTitleWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setChartTitleFont(const QFont& font)
{
	ui->chartTitleWidget->setFont(font);
}

QFont Graph2dHybridWindowFontSettingDialog::legendsFont() const
{
	return ui->legendWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setLegendsFont(const QFont& font)
{
	ui->legendWidget->setFont(font);
}

QFont Graph2dHybridWindowFontSettingDialog::xAxisTitleFont() const
{
	return ui->xAxisTitleWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setXAxisTitleFont(const QFont& font)
{
	ui->xAxisTitleWidget->setFont(font);
}

QFont Graph2dHybridWindowFontSettingDialog::xAxisTickFont() const
{
	return ui->xAxisTickWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setXAxisTickFont(const QFont& font)
{
	ui->xAxisTickWidget->setFont(font);
}

QFont Graph2dHybridWindowFontSettingDialog::yAxisTitleFont() const
{
	return ui->yAxisTitleWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setYAxisTitleFont(const QFont& font)
{
	ui->yAxisTitleWidget->setFont(font);
}

QFont Graph2dHybridWindowFontSettingDialog::yAxisTickFont() const
{
	return ui->yAxisTickWidget->font();
}

void Graph2dHybridWindowFontSettingDialog::setYAxisTickFont(const QFont& font)
{
	ui->yAxisTickWidget->setFont(font);
}
