#include "ui_graph2dwindowmarkersettingdialog.h"

#include "graph2dwindowmarkersettingdialog.h"

#include <guicore/project/colorsource.h>

#include <qwt_plot.h>

Graph2dWindowMarkerSettingDialog::Graph2dWindowMarkerSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dWindowMarkerSettingDialog)
{
	m_item = nullptr;
	m_colorSource = new ColorSource(nullptr);
	ui->setupUi(this);
	connect(ui->markerListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(itemChanged(int)));

	connect(ui->oriHorizontalRadioButton, SIGNAL(clicked()), this, SLOT(oriantationChanged()));
	connect(ui->oriVerticalRadioButton, SIGNAL(clicked()), this, SLOT(oriantationChanged()));

	connect(ui->markerValueLineEdit, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));

	connect(ui->labelAutoRadioButton, SIGNAL(clicked()), this, SLOT(labelChanged()));
	connect(ui->labelCustomRadioButton, SIGNAL(clicked()), this, SLOT(labelChanged()));
	connect(ui->customLabelLineEdit, SIGNAL(textChanged(QString)), this, SLOT(labelChanged()));

	connect(ui->axisLeftRadioButton, SIGNAL(clicked()), this, SLOT(axisChanged()));
	connect(ui->axisRightRadioButton, SIGNAL(clicked()), this, SLOT(axisChanged()));

	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
	connect(ui->lineStyleComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(styleChanged(int)));

	connect(ui->showLineCheckBox, SIGNAL(toggled(bool)), this, SLOT(showLineChanged(bool)));
	connect(ui->posCenterRadioButton, SIGNAL(toggled(bool)), this, SLOT(alignCenterChanged(bool)));

	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addItem()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeItem()));
}

Graph2dWindowMarkerSettingDialog::~Graph2dWindowMarkerSettingDialog()
{
	delete ui;
	delete m_colorSource;
}

void Graph2dWindowMarkerSettingDialog::setSetting(const Graph2dWindowMarkerSetting& setting)
{
	m_setting = setting;
	setupList();
}

void Graph2dWindowMarkerSettingDialog::setupList()
{
	ui->markerListWidget->clear();
	for (auto it = m_setting.items().begin(); it != m_setting.items().end(); ++it) {
		Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item = *it;
		ui->markerListWidget->addItem(label(item));
	}
	if (m_setting.items().count() > 0) {
		ui->markerListWidget->setCurrentRow(0);
		itemChanged(0);
	} else {
		itemChanged(-1);
	}
}

void Graph2dWindowMarkerSettingDialog::updateLabel()
{
	static bool updating = false;
	if (updating) {return;}
	updating = true;
	if (m_item == nullptr) {
		updating = false;
		return;
	}
	QString label;
	if (m_item->labelMode() == Graph2dWindowMarkerSetting::lmAuto) {
		label = autoLabel();
		ui->customLabelLineEdit->setText(label);
	} else {
		label = ui->customLabelLineEdit->text();
	}
	m_item->setLabel(label);
	QListWidgetItem* cItem = ui->markerListWidget->currentItem();
	if (cItem != nullptr) {cItem->setText(label);}
	updating = false;
}

const QString Graph2dWindowMarkerSettingDialog::label(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item)
{
	if (item.labelMode() == Graph2dWindowMarkerSetting::lmAuto) {
		return autoLabel(item);
	} else {
		return item.customLabel();
	}
}

const QString Graph2dWindowMarkerSettingDialog::autoLabel(const Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item)
{
	QString label("%1 = %2");
	QString var;
	if (item.orientation() == Graph2dWindowMarkerSetting::oHorizontal) {
		var = tr("y");
	} else {
		var = m_xAxisLabel;
	}
	return label.arg(var).arg(item.value());
}

const QString Graph2dWindowMarkerSettingDialog::autoLabel()
{
	return autoLabel(* m_item);
}

void Graph2dWindowMarkerSettingDialog::itemChanged(int index)
{
	if (index == -1) {
		m_item = nullptr;
		ui->orientationGroupBox->setEnabled(false);
		ui->oriHorizontalRadioButton->setChecked(true);
		ui->markerValueGroupBox->setEnabled(false);
		ui->markerValueLineEdit->clear();
		ui->labelGroupBox->setEnabled(false);
		ui->labelAutoRadioButton->setChecked(true);
		ui->axisGroupBox->setEnabled(false);
		ui->axisLeftRadioButton->setChecked(true);
		ui->styleGroupBox->setEnabled(false);
		ui->colorWidget->setColor(Qt::black);
		ui->lineStyleComboBox->setCurrentIndex(0);
		ui->showLineCheckBox->setChecked(true);
		ui->removePushButton->setDisabled(true);
		return;
	}
	QList<Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem>& list = m_setting.items();
	Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& item = list[index];
	m_item = &item;
	ui->orientationGroupBox->setEnabled(true);
	if (m_item->orientation() == Graph2dWindowMarkerSetting::oHorizontal) {
		ui->oriHorizontalRadioButton->setChecked(true);
	} else {
		ui->oriVerticalRadioButton->setChecked(true);
	}
	ui->markerValueGroupBox->setEnabled(true);
	ui->markerValueLineEdit->setValue(m_item->value());
	ui->labelGroupBox->setEnabled(true);
	if (m_item->labelMode() == Graph2dWindowMarkerSetting::lmAuto) {
		ui->labelAutoRadioButton->setChecked(true);
		ui->customLabelLineEdit->setText(autoLabel());
	} else {
		ui->labelCustomRadioButton->setChecked(true);
		ui->customLabelLineEdit->setText(m_item->customLabel());
	}
	ui->axisGroupBox->setEnabled(true);
	if (m_item->axis() == QwtPlot::yLeft) {
		ui->axisLeftRadioButton->setChecked(true);
	} else {
		ui->axisRightRadioButton->setChecked(true);
	}
	ui->styleGroupBox->setEnabled(true);
	ui->colorWidget->setColor(item.color());
	if (m_item->penStyle() == Qt::SolidLine) {
		ui->lineStyleComboBox->setCurrentIndex(0);
	} else if (m_item->penStyle() == Qt::DashLine) {
		ui->lineStyleComboBox->setCurrentIndex(1);
	} else if (m_item->penStyle() == Qt::DotLine) {
		ui->lineStyleComboBox->setCurrentIndex(2);
	} else if (m_item->penStyle() == Qt::DashDotLine) {
		ui->lineStyleComboBox->setCurrentIndex(3);
	}
	if (m_item->alignCenter()) {
		ui->posCenterRadioButton->setChecked(true);
	} else {
		ui->posAboveRadioButton->setChecked(true);
	}
	ui->showLineCheckBox->setChecked(m_item->showLine());
	ui->removePushButton->setEnabled(true);
}

void Graph2dWindowMarkerSettingDialog::oriantationChanged()
{
	if (m_item == nullptr) {return;}
	if (ui->oriHorizontalRadioButton->isChecked()) {
		m_item->setOrientation(Graph2dWindowMarkerSetting::oHorizontal);
	} else {
		m_item->setOrientation(Graph2dWindowMarkerSetting::oVertical);
	}
	updateLabel();
}

void Graph2dWindowMarkerSettingDialog::valueChanged(double val)
{
	if (m_item == nullptr) {return;}
	m_item->setValue(val);
	updateLabel();
}

void Graph2dWindowMarkerSettingDialog::labelChanged()
{
	if (m_item == nullptr) {return;}
	if (ui->labelAutoRadioButton->isChecked()) {
		m_item->setLabelMode(Graph2dWindowMarkerSetting::lmAuto);
	} else {
		m_item->setLabelMode(Graph2dWindowMarkerSetting::lmCustom);
		m_item->setCustomLabel(ui->customLabelLineEdit->text());
	}
	updateLabel();
}

void Graph2dWindowMarkerSettingDialog::addItem()
{
	QList<Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem>& items = m_setting.items();
	items.append(Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem());
	Graph2dWindowMarkerSetting::Graph2dWindowMarkerSettingItem& newitem = items[items.count() - 1];
	m_item = &newitem;
	m_item->setLabel(autoLabel());
	QColor col = m_colorSource->getColor(items.count() - 1);
	m_item->setColor(col);
	ui->markerValueLineEdit->blockSignals(true);
	ui->markerListWidget->addItem(autoLabel());
	ui->markerValueLineEdit->blockSignals(false);
	ui->markerListWidget->setCurrentRow(items.count() - 1);
}

void Graph2dWindowMarkerSettingDialog::removeItem()
{
	int index = ui->markerListWidget->currentRow();
	if (index >= m_setting.items().count()) {return;}
	m_setting.items().removeAt(index);
	ui->markerListWidget->blockSignals(true);
	QListWidgetItem* item = ui->markerListWidget->takeItem(index);
	delete item;
	ui->markerListWidget->blockSignals(false);
	int newindex = index;
	if (newindex > ui->markerListWidget->count() - 1) {
		newindex = ui->markerListWidget->count() - 1;
	}
	itemChanged(newindex);
}

void Graph2dWindowMarkerSettingDialog::axisChanged()
{
	if (m_item == nullptr) {return;}
	if (ui->axisLeftRadioButton->isChecked()) {
		m_item->setAxis(QwtPlot::yLeft);
	} else {
		m_item->setAxis(QwtPlot::yRight);
	}
}
void Graph2dWindowMarkerSettingDialog::styleChanged(int index)
{
	if (m_item == nullptr) {return;}
	Qt::PenStyle penStyle;
	switch (index) {
	case 0:
		penStyle = Qt::SolidLine;
		break;
	case 1:
		penStyle = Qt::DashLine;
		break;
	case 2:
		penStyle = Qt::DotLine;
		break;
	case 3:
		penStyle = Qt::DashDotLine;
		break;
	default:
		penStyle = Qt::SolidLine;
	}
	m_item->setPenStyle(penStyle);
}
void Graph2dWindowMarkerSettingDialog::colorChanged(const QColor& col)
{
	if (m_item == nullptr) {return;}
	m_item->setColor(col);
}

void Graph2dWindowMarkerSettingDialog::showLineChanged(bool show)
{
	if (m_item == nullptr) {return;}
	m_item->setShowLine(show);
}

void Graph2dWindowMarkerSettingDialog::alignCenterChanged(bool center)
{
	if (m_item == nullptr) {return;}
	m_item->setAlignCenter(center);
}
