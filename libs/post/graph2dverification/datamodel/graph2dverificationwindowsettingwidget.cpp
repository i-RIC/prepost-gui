#include "ui_graph2dverificationwindowsettingwidget.h"

#include "../graph2dverificationwindowresultsetting.h"
#include "graph2dverificationwindowsettingwidget.h"

Graph2dVerificationWindowSettingWidget::Graph2dVerificationWindowSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::Graph2dVerificationWindowSettingWidget)
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

Graph2dVerificationWindowSettingWidget::~Graph2dVerificationWindowSettingWidget()
{
	delete ui;
}

void Graph2dVerificationWindowSettingWidget::setSetting(Graph2dVerificationWindowResultSetting::Setting* setting)
{
	m_setting = setting;
	this->blockSignals(true);
	ui->labelLineEdit->setText(setting->name());
	if (setting->axisSide() == Graph2dVerificationWindowResultSetting::asLeft) {
		ui->yAxisLeftRadioButton->setChecked(true);
	} else {
		ui->yAxisRightRadioButton->setChecked(true);
	}
	ui->lineWidthSpinBox->setValue(setting->lineWidth());
	ui->colorWidget->setColor(setting->customColor());
	if (setting->styleType() == Graph2dVerificationWindowResultSetting::stLine) {
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

void Graph2dVerificationWindowSettingWidget::setId(const QString& id)
{
	ui->idWidget->setText(id);
}

void Graph2dVerificationWindowSettingWidget::setupForResult()
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

void Graph2dVerificationWindowSettingWidget::setupForResultCopy()
{
	ui->displayStyleLineRadioButton->setChecked(true);
	ui->displayStyleGroupBox->setHidden(true);
	ui->symbolStyleLabel->setHidden(true);
	ui->symbolStyleComboBox->setHidden(true);
}

void Graph2dVerificationWindowSettingWidget::setupForExternalData()
{
}

void Graph2dVerificationWindowSettingWidget::updateLabel(const QString& label)
{
	m_setting->setName(label);
	emit labelChanged(label);
}

void Graph2dVerificationWindowSettingWidget::updateAxis()
{
	if (ui->yAxisLeftRadioButton->isChecked()) {
		m_setting->setAxisSide(Graph2dVerificationWindowResultSetting::asLeft);
	} else {
		m_setting->setAxisSide(Graph2dVerificationWindowResultSetting::asRight);
	}
}

void Graph2dVerificationWindowSettingWidget::updateDisplayStyle()
{
	if (ui->displayStyleLineRadioButton->isChecked()) {
		m_setting->setStyleType(Graph2dVerificationWindowResultSetting::stLine);
	} else {
		m_setting->setStyleType(Graph2dVerificationWindowResultSetting::stSymbol);
	}
}

void Graph2dVerificationWindowSettingWidget::updateLineStyle(int index)
{
	Graph2dVerificationWindowResultSetting::LineType lt = static_cast<Graph2dVerificationWindowResultSetting::LineType>(index);
	m_setting->setLineType(lt);
}

void Graph2dVerificationWindowSettingWidget::updateLineWidth(int width)
{
	m_setting->setLineWidth(width);
}

void Graph2dVerificationWindowSettingWidget::updateSymbolStyle(int index)
{
	Graph2dVerificationWindowResultSetting::SymbolType st = static_cast<Graph2dVerificationWindowResultSetting::SymbolType>(index);
	m_setting->setSymbolType(st);
}

void Graph2dVerificationWindowSettingWidget::updateColor(const QColor& color)
{
	m_setting->setCustomColor(color);
}

void Graph2dVerificationWindowSettingWidget::updateBarChart(bool bar)
{
	m_setting->setBarChart(bar);
}

void Graph2dVerificationWindowSettingWidget::clear()
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
