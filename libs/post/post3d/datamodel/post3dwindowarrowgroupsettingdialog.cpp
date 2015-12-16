#include "ui_post3dwindowarrowgroupsettingdialog.h"

#include "post3dwindowarrowgroupsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/arrowsettingcontainer.h>

#include <vtkPointData.h>

Post3dWindowArrowGroupSettingDialog::Post3dWindowArrowGroupSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post3dWindowArrowGroupSettingDialog)
{
	ui->setupUi(this);
	ui->faceListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->faceSettingWidget->setEnabled(false);

	connect(ui->faceAddButton, SIGNAL(clicked()), this, SLOT(addFaceSetting()));
	connect(ui->faceRemoveButton, SIGNAL(clicked()), this, SLOT(removeFaceSetting()));
	connect(ui->faceListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(switchFaceSetting(QListWidgetItem*,QListWidgetItem*)));
	connect(ui->faceListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelectedNumber()));
	connect(ui->faceSettingWidget, SIGNAL(settingChanged()), this, SLOT(updateFaceMap()));

	connect(ui->samplingAllRadioButton, SIGNAL(toggled(bool)), this, SLOT(allSamplingToggled(bool)));

	m_isRemoving = false;
}

Post3dWindowArrowGroupSettingDialog::~Post3dWindowArrowGroupSettingDialog()
{
	delete ui;
}

void Post3dWindowArrowGroupSettingDialog::setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map)
{
	m_faceMap = map;
	if (m_faceMap.size() < 1) { return; }

	ui->faceSettingWidget->setEnabled(true);
	for (auto it = m_faceMap.begin(); it != m_faceMap.end(); ++it) {
		ui->faceListWidget->addItem(it.key());
	}
	ui->faceListWidget->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
}

const QMap<QString, Post3dWindowFaceDataItem::Setting>& Post3dWindowArrowGroupSettingDialog::faceMap()
{
	return m_faceMap;
}

void Post3dWindowArrowGroupSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	ui->faceSettingWidget->setZoneData(zoneData);

	SolverDefinitionGridType* gType = zoneData->gridType();
	vtkPointData* pd = zoneData->data()->GetPointData();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_targets), ui->physicalValueComboBox);

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_targets.size() < 2) {
		ui->physicalValueLabel->hide();
		ui->physicalValueComboBox->hide();
	}
}

void Post3dWindowArrowGroupSettingDialog::addFaceSetting()
{
	ui->faceSettingWidget->setEnabled(true);

	// add an item to the face list
	int idx = 0;
	bool ok = false;
	QString label;
	while (! ok) {
		label = QString(tr("Face%1")).arg(++idx, 3, 10, QChar('0'));
		if (ui->faceListWidget->findItems(label, 0).size() == 0) {
			ok = true;
		}
	}
	ui->faceListWidget->insertItem(idx - 1, label);

	// add a setting to m_faceMap
	Post3dWindowFaceDataItem::Setting s;
	s.direction = Post3dWindowFaceDataItem::dirK;
	s.enabled = true;
	int range[6];
	ui->faceSettingWidget->sliderRange(range);
	s.iMin = range[0];
	s.iMax = range[1];
	s.jMin = range[2];
	s.jMax = range[3];
	s.kMin = s.kMax = range[4];
	m_faceMap.insert(label, s);

	ui->faceListWidget->clearSelection();
	ui->faceListWidget->setCurrentRow(idx - 1, QItemSelectionModel::SelectCurrent);
	ui->faceListWidget->scrollToItem(ui->faceListWidget->item(idx - 1));
}

void Post3dWindowArrowGroupSettingDialog::removeFaceSetting()
{
	m_isRemoving = true;

	QList<QListWidgetItem*> items = ui->faceListWidget->selectedItems();
	for (auto it = items.begin(); it != items.end(); ++it) {
		QListWidgetItem* widgetItem = *it;
		m_faceMap.remove(widgetItem->text());
	}
	qDeleteAll(items);

	if (ui->faceListWidget->count() < 1) {
		ui->faceSettingWidget->setEnabled(false);
	}

	m_isRemoving = false;
}

void Post3dWindowArrowGroupSettingDialog::switchFaceSetting(QListWidgetItem* current, QListWidgetItem* /*previous*/)
{
	if (current == 0) { return; }

	QString currentLabel = current->text();
	ui->faceSettingWidget->setSetting(m_faceMap.value(currentLabel));
	ui->faceListWidget->setCurrentItem(current, QItemSelectionModel::SelectCurrent);
}

void Post3dWindowArrowGroupSettingDialog::allSamplingToggled(bool toggled)
{
	if (toggled) {
		ui->sampleRateSpinBox->setValue(1);
	}
}

void Post3dWindowArrowGroupSettingDialog::checkSelectedNumber()
{
	QList<QListWidgetItem*> items = ui->faceListWidget->selectedItems();
	int num = items.size();

	if (num < 2) {
		ui->faceSettingWidget->setMultiSelected(false);
		// temporary?
		ui->faceSettingWidget->setEnabled(true);
	} else {
		ui->faceSettingWidget->setMultiSelected(true);
		// temporary?
		ui->faceSettingWidget->setEnabled(false);
	}
}

void Post3dWindowArrowGroupSettingDialog::updateFaceMap()
{
	m_faceMap.insert(ui->faceListWidget->currentItem()->text(), ui->faceSettingWidget->setting());
}

ArrowSettingContainer Post3dWindowArrowGroupSettingDialog::setting() const
{
	ArrowSettingContainer ret;

	ret.target = ui->physicalValueComboBox->currentText();

	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::All;
	} else {
		ret.samplingMode = ArrowSettingContainer::SamplingMode::Rate;
	}
	ret.samplingRate = ui->sampleRateSpinBox->value();

	if (ui->specificRadioButton->isChecked()) {
		ret.colorMode = ArrowSettingContainer::ColorMode::Custom;
	}	else if (ui->scalarRadioButton->isChecked()) {
		ret.colorMode = ArrowSettingContainer::ColorMode::ByScalar;
	}
	ret.customColor = ui->colorEditWidget->color();
	ret.colorTarget = ui->scalarComboBox->currentText();

	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Auto;
	} else {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Custom;
	}
	ret.standardValue = ui->stdValueSpinBox->value();
	ret.legendLength = ui->legendLengthSpinBox->value();
	ret.minimumValue = ui->minValueSpinBox->value();

	ret.arrowSize = ui->arrowSizeSpinBox->value();
	ret.lineWidth = ui->lineWidthSpinBox->value();

	return ret;
}

void Post3dWindowArrowGroupSettingDialog::setSetting(const ArrowSettingContainer& s)
{
	ui->physicalValueComboBox->setCurrentText(s.target);

	switch (s.samplingMode.value()) {
	case ArrowSettingContainer::SamplingMode::Rate:
		ui->samplingSkipRadioButton->setChecked(true);
		break;
	default:
		ui->samplingAllRadioButton->setChecked(true);
		break;
	}
	ui->sampleRateSpinBox->setValue(s.samplingRate);

	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		ui->specificRadioButton->setChecked(true);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
	ui->colorEditWidget->setColor(s.customColor);
	ui->scalarComboBox->setCurrentText(s.colorTarget);

	switch (s.lengthMode.value()) {
	case ArrowSettingContainer::LengthMode::Auto:
		ui->lengthAutoCheckBox->setChecked(true);
		break;
	default:
		ui->lengthAutoCheckBox->setChecked(false);
		break;
	}
	ui->stdValueSpinBox->setValue(s.standardValue);
	ui->legendLengthSpinBox->setValue(s.legendLength);
	ui->minValueSpinBox->setValue(s.minimumValue);

	ui->arrowSizeSpinBox->setValue(s.arrowSize);
	ui->lineWidthSpinBox->setValue(s.lineWidth);
}
