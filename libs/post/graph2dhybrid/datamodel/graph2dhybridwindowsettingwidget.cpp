#include "../graph2dhybridwindowresultsetting.h"
#include "graph2dhybridwindowsettingwidget.h"
#include "ui_graph2dhybridwindowsettingwidget.h"

Graph2dHybridWindowSettingWidget::Graph2dHybridWindowSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Graph2dHybridWindowSettingWidget)
{
	ui->setupUi(this);
	connect(ui->labelLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateLabel(QString)));
	connect(ui->yAxisLeftRadioButton, SIGNAL(clicked()), this, SLOT(updateAxis()));
	connect(ui->yAxisRightRadioButton, SIGNAL(clicked()), this, SLOT(updateAxis()));
	connect(ui->displayStyleLineRadioButton , SIGNAL(clicked()), this, SLOT(updateDisplayStyle()));
	connect(ui->displayStyleSymbolRadioButton, SIGNAL(clicked()), this, SLOT(updateDisplayStyle()));
	connect(ui->lineStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateLineStyle(int)));
	connect(ui->lineWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateLineWidth(int)));
	connect(ui->symbolStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSymbolStyle(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(updateColor(QColor)));
	connect(ui->barChartCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateBarChart(bool)));
	connect(ui->deleteButton,SIGNAL(clicked()), this, SIGNAL(deleteButtonClicked()));
}

Graph2dHybridWindowSettingWidget::~Graph2dHybridWindowSettingWidget()
{
	delete ui;
}

void Graph2dHybridWindowSettingWidget::setSetting(Graph2dHybridWindowResultSetting::Setting* setting)
{
	m_setting = setting;
	this->blockSignals(true);
	ui->labelLineEdit->setText(setting->name());
	if (setting->axisSide() == Graph2dHybridWindowResultSetting::asLeft) {
		ui->yAxisLeftRadioButton->setChecked(true);
	} else {
		ui->yAxisRightRadioButton->setChecked(true);
	}
	ui->lineWidthSpinBox->setValue(setting->lineWidth());
	ui->colorWidget->setColor(setting->customColor());
	if (setting->styleType() == Graph2dHybridWindowResultSetting::stLine) {
		ui->displayStyleLineRadioButton->setChecked(true);
	} else {
		ui->displayStyleSymbolRadioButton->setChecked(true);
	}
	int linetype = static_cast<int>(setting->lineType());
	ui->lineStyleComboBox->setCurrentIndex(linetype);
	int symboltype = static_cast<int>(setting->symbolType());
	ui->symbolStyleComboBox->setCurrentIndex(symboltype);
	ui->barChartCheckBox->setChecked(setting->isBarChart());
	this->blockSignals(false);
}

void Graph2dHybridWindowSettingWidget::setId(const QString& id)
{
	ui->idWidget->setText(id);
}

void Graph2dHybridWindowSettingWidget::setupForResult()
{
	ui->labelLineEdit->setDisabled(true);
	ui->displayStyleLineRadioButton->setChecked(true);
	ui->displayStyleGroupBox->setHidden(true);
	ui->symbolStyleLabel->setHidden(true);
	ui->symbolStyleComboBox->setHidden(true);
	ui->idLabel->setHidden(true);
	ui->idWidget->setHidden(true);
	ui->deleteButton->setHidden(true);
}

void Graph2dHybridWindowSettingWidget::setupForResultCopy()
{
	ui->displayStyleLineRadioButton->setChecked(true);
	ui->displayStyleGroupBox->setHidden(true);
	ui->symbolStyleLabel->setHidden(true);
	ui->symbolStyleComboBox->setHidden(true);
}

void Graph2dHybridWindowSettingWidget::setupForExternalData()
{
}

void Graph2dHybridWindowSettingWidget::updateLabel(const QString& label)
{
	m_setting->setName(label);
	emit labelChanged(label);
}

void Graph2dHybridWindowSettingWidget::updateAxis()
{
	if (ui->yAxisLeftRadioButton->isChecked()) {
		m_setting->setAxisSide(Graph2dHybridWindowResultSetting::asLeft);
	} else {
		m_setting->setAxisSide(Graph2dHybridWindowResultSetting::asRight);
	}
}

void Graph2dHybridWindowSettingWidget::updateDisplayStyle()
{
	if (ui->displayStyleLineRadioButton->isChecked()) {
		m_setting->setStyleType(Graph2dHybridWindowResultSetting::stLine);
	} else {
		m_setting->setStyleType(Graph2dHybridWindowResultSetting::stSymbol);
	}
}

void Graph2dHybridWindowSettingWidget::updateLineStyle(int index)
{
	Graph2dHybridWindowResultSetting::LineType lt = static_cast<Graph2dHybridWindowResultSetting::LineType>(index);
	m_setting->setLineType(lt);
}

void Graph2dHybridWindowSettingWidget::updateLineWidth(int width)
{
	m_setting->setLineWidth(width);
}

void Graph2dHybridWindowSettingWidget::updateSymbolStyle(int index)
{
	Graph2dHybridWindowResultSetting::SymbolType st = static_cast<Graph2dHybridWindowResultSetting::SymbolType>(index);
	m_setting->setSymbolType(st);
}

void Graph2dHybridWindowSettingWidget::updateColor(const QColor& color)
{
	m_setting->setCustomColor(color);
}

void Graph2dHybridWindowSettingWidget::updateBarChart(bool bar)
{
	m_setting->setBarChart(bar);
}

void Graph2dHybridWindowSettingWidget::clear()
{
	ui->labelLineEdit->clear();
	ui->yAxisLeftRadioButton->setChecked(true);
	ui->lineStyleComboBox->setCurrentIndex(0);
	ui->displayStyleLineRadioButton->setChecked(true);
	ui->lineWidthSpinBox->setValue(1);
	ui->colorWidget->setColor(Qt::black);
	ui->symbolStyleComboBox->setCurrentIndex(0);
	ui->barChartCheckBox->setChecked(false);
}
