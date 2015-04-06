#include "graph2dscatteredwindowsettingwidget.h"
#include "ui_graph2dscatteredwindowsettingwidget.h"
#include "graph2dscatteredwindowresultsetting.h"

Graph2dScatteredWindowSettingWidget::Graph2dScatteredWindowSettingWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::Graph2dScatteredWindowSettingWidget)
{
	ui->setupUi(this);
	connect(ui->labelLineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateLabel(QString)));
	connect(ui->yAxisLeftRadioButton, SIGNAL(clicked()), this, SLOT(updateAxis()));
	connect(ui->yAxisRightRadioButton, SIGNAL(clicked()), this, SLOT(updateAxis()));
	connect(ui->symbolStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSymbolStyle(int)));
	connect(ui->symbolSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateSymbolSize(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(updateColor(QColor)));
}

Graph2dScatteredWindowSettingWidget::~Graph2dScatteredWindowSettingWidget()
{
	delete ui;
}

void Graph2dScatteredWindowSettingWidget::setSetting(Graph2dScatteredWindowResultSetting::Setting* setting)
{
	m_setting = setting;
	this->blockSignals(true);
	ui->labelLineEdit->setText(setting->name());
	if (setting->axisSide() == Graph2dScatteredWindowResultSetting::asLeft){
		ui->yAxisLeftRadioButton->setChecked(true);
	} else {
		ui->yAxisRightRadioButton->setChecked(true);
	}
	ui->colorWidget->setColor(setting->customColor());
	int symboltype = static_cast<int>(setting->symbolType());
	ui->symbolStyleComboBox->setCurrentIndex(symboltype);
	ui->symbolSizeSpinBox->setValue(m_setting->symbolSize());
	this->blockSignals(false);
}

void Graph2dScatteredWindowSettingWidget::updateLabel(const QString& label)
{
	m_setting->setName(label);
	emit labelChanged(label);
}

void Graph2dScatteredWindowSettingWidget::updateAxis()
{
	if (ui->yAxisLeftRadioButton->isChecked()){
		m_setting->setAxisSide(Graph2dScatteredWindowResultSetting::asLeft);
	} else {
		m_setting->setAxisSide(Graph2dScatteredWindowResultSetting::asRight);
	}
}

void Graph2dScatteredWindowSettingWidget::updateSymbolStyle(int index)
{
	Graph2dScatteredWindowResultSetting::SymbolType st = static_cast<Graph2dScatteredWindowResultSetting::SymbolType>(index);
	m_setting->setSymbolType(st);
}

void Graph2dScatteredWindowSettingWidget::updateSymbolSize(int size)
{
	m_setting->setSymbolSize(size);
}

void Graph2dScatteredWindowSettingWidget::updateColor(const QColor& color)
{
	m_setting->setCustomColor(color);
}

void Graph2dScatteredWindowSettingWidget::clear()
{
	ui->labelLineEdit->clear();
	ui->yAxisLeftRadioButton->setChecked(true);
	ui->colorWidget->setColor(Qt::black);
	ui->symbolStyleComboBox->setCurrentIndex(0);
}
