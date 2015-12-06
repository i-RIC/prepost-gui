#include "ui_post3dwindowarrowgroupsettingdialog.h"

#include "post3dwindowarrowgroupsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/arrowsettingcontainer.h>
#include <misc/stringtool.h>

#include <vtkPointData.h>

Post3dWindowArrowGroupSettingDialog::Post3dWindowArrowGroupSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post3dWindowArrowGroupSettingDialog)
{
	ui->setupUi(this);
	ui->faceListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->faceSettingWidget->setEnabled(false);

	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));

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

void Post3dWindowArrowGroupSettingDialog::setColor(const QColor& color)
{
	ui->colorEditWidget->setColor(color);
}

QColor Post3dWindowArrowGroupSettingDialog::color()
{
	return ui->colorEditWidget->color();
}

void Post3dWindowArrowGroupSettingDialog::setMapping(Post3dWindowArrowGroupDataItem::Mapping m)
{
	switch (m) {
	case Post3dWindowArrowGroupDataItem::Specific:
		ui->specificRadioButton->setChecked(true);
		break;
	case Post3dWindowArrowGroupDataItem::Scalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
}

Post3dWindowArrowGroupDataItem::Mapping Post3dWindowArrowGroupSettingDialog::mapping()
{
	if (ui->specificRadioButton->isChecked()) { return Post3dWindowArrowGroupDataItem::Specific; }
	if (ui->scalarRadioButton->isChecked()) { return Post3dWindowArrowGroupDataItem::Scalar; }
	// default
	return Post3dWindowArrowGroupDataItem::Specific;
}

void Post3dWindowArrowGroupSettingDialog::setScalarValueName(const std::string& name)
{
	int index = ui->scalarComboBox->findText(name.c_str());
	ui->scalarComboBox->setCurrentIndex(index);
}

std::string Post3dWindowArrowGroupSettingDialog::scalarValueName()
{
	return iRIC::toStr(ui->scalarComboBox->currentText());
}

void Post3dWindowArrowGroupSettingDialog::setSampleRate(int rate)
{
	if (rate == 1) {
		ui->samplingAllRadioButton->setChecked(true);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
	}
	ui->sampleRateSpinBox->setValue(rate);
}

int Post3dWindowArrowGroupSettingDialog::sampleRate()
{
	return ui->sampleRateSpinBox->value();
}

void Post3dWindowArrowGroupSettingDialog::setCurrentSolution(const std::string& sol)
{
	auto it = std::find(m_solutions.begin(), m_solutions.end(), sol);
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->physicalValueComboBox->setCurrentIndex(it - m_solutions.begin());
}

const std::string Post3dWindowArrowGroupSettingDialog::currentSolution() const
{
	return iRIC::toStr(ui->physicalValueComboBox->currentText());
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

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_solutions), ui->physicalValueComboBox);

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_solutions.size() < 2) {
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

void Post3dWindowArrowGroupSettingDialog::solutionChanged(int /*index*/)
{
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

void Post3dWindowArrowGroupSettingDialog::setLengthMode(Post3dWindowArrowGroupDataItem::LengthMode lm)
{
	if (lm == Post3dWindowArrowGroupDataItem::lenAuto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}
}

Post3dWindowArrowGroupDataItem::LengthMode Post3dWindowArrowGroupSettingDialog::lengthMode()
{
	if (ui->lengthAutoCheckBox->isChecked()) {
		return Post3dWindowArrowGroupDataItem::lenAuto;
	} else {
		return Post3dWindowArrowGroupDataItem::lenCustom;
	}
}

void Post3dWindowArrowGroupSettingDialog::setStandardValue(double stdVal)
{
	ui->stdValueSpinBox->setValue(stdVal);
}

double Post3dWindowArrowGroupSettingDialog::standardValue()
{
	return ui->stdValueSpinBox->value();
}

void Post3dWindowArrowGroupSettingDialog::setLegendLength(int len)
{
	ui->legendLengthSpinBox->setValue(len);
}

int Post3dWindowArrowGroupSettingDialog::legendLength()
{
	return ui->legendLengthSpinBox->value();
}

void Post3dWindowArrowGroupSettingDialog::setMinimumValue(double minVal)
{
	return ui->minValueSpinBox->setValue(minVal);
}

double Post3dWindowArrowGroupSettingDialog::minimumValue()
{
	return ui->minValueSpinBox->value();
}

void Post3dWindowArrowGroupSettingDialog::setArrowSetting(const ArrowSettingContainer& arrowSetting)
{
	ui->arrowSizeSpinBox->setValue(arrowSetting.arrowSize());
	ui->lineWidthSpinBox->setValue(arrowSetting.lineWidth());
}

ArrowSettingContainer Post3dWindowArrowGroupSettingDialog::arrowSetting()
{
	ArrowSettingContainer c;
	c.setArrowSize(ui->arrowSizeSpinBox->value());
	c.setLineWidth(ui->lineWidthSpinBox->value());
	return c;
}
