#include "ui_post3dwindowcontourgroupsettingdialog.h"

#include "post3dwindowcontourgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/scalarbardialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkPointData.h>

Post3dWindowContourGroupSettingDialog::Post3dWindowContourGroupSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post3dWindowContourGroupSettingDialog)
{
	ui->setupUi(this);
	ui->faceListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->faceSettingWidget->setEnabled(false);
	ui->contourWidget->hidePointsRadioButton();
	ui->colormapWidget->setContainer(&m_lookupTable);

	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));

	connect(ui->faceAddButton, SIGNAL(clicked()), this, SLOT(addFaceSetting()));
	connect(ui->faceRemoveButton, SIGNAL(clicked()), this, SLOT(removeFaceSetting()));
	connect(ui->faceListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(switchFaceSetting(QListWidgetItem*,QListWidgetItem*)));
	connect(ui->faceListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelectedNumber()));
	connect(ui->faceSettingWidget, SIGNAL(settingChanged()), this, SLOT(updateFaceMap()));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showColorBarDialog()));

	m_isRemoving = false;
}

Post3dWindowContourGroupSettingDialog::~Post3dWindowContourGroupSettingDialog()
{
	delete ui;
}

void Post3dWindowContourGroupSettingDialog::setGridTypeDataItem(Post3dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

void Post3dWindowContourGroupSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	ui->faceSettingWidget->setZoneData(zoneData);

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zoneData->data()->GetPointData());
	ComboBoxTool::setupItems(m_gridTypeDataItem->gridType()->solutionCaptions(m_targets), ui->physicalValueComboBox);
}

void Post3dWindowContourGroupSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titleMap)
{
	m_colorBarTitleMap = titleMap;
}

ScalarSettingContainer Post3dWindowContourGroupSettingDialog::scalarSetting() const
{
	ScalarSettingContainer ret = m_scalarSetting;

	ret.target = ui->physicalValueComboBox->currentText();
	ret.contour = ui->contourWidget->contour();
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();
	ret.fillUpper = ui->colormapWidget->fillUpper();
	ret.fillLower = ui->colormapWidget->fillLower();

	return ret;
}

void Post3dWindowContourGroupSettingDialog::setScalarSetting(const ScalarSettingContainer& setting)
{
	m_scalarSetting = setting;

	ui->physicalValueComboBox->setCurrentText(setting.target);
	ui->contourWidget->setContour(setting.contour);
	ui->colormapWidget->setDivisionNumber(setting.numberOfDivisions);
	ui->colormapWidget->setFillLower(setting.fillLower);
	ui->colormapWidget->setFillUpper(setting.fillUpper);
}

LookupTableContainer Post3dWindowContourGroupSettingDialog::lookupTable() const
{
	return m_lookupTable;
}

void Post3dWindowContourGroupSettingDialog::setLookupTable(const LookupTableContainer& c)
{
	m_lookupTable = c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

const QMap<QString, Post3dWindowFaceDataItem::Setting>& Post3dWindowContourGroupSettingDialog::faceMap() const
{
	return m_faceMap;
}

void Post3dWindowContourGroupSettingDialog::setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map)
{
	m_faceMap = map;
	if (m_faceMap.size() < 1) { return; }

	ui->faceSettingWidget->setEnabled(true);
	for (auto it = m_faceMap.begin(); it != m_faceMap.end(); ++it) {
		ui->faceListWidget->addItem(it.key());
	}
	ui->faceListWidget->setCurrentRow(0, QItemSelectionModel::SelectCurrent);
}

QString Post3dWindowContourGroupSettingDialog::scalarBarTitle() const
{
	return m_colorBarTitleMap[target()];
}

void Post3dWindowContourGroupSettingDialog::accept()
{
	ui->colormapWidget->save();
	QDialog::accept();
}

void Post3dWindowContourGroupSettingDialog::addFaceSetting()
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

void Post3dWindowContourGroupSettingDialog::removeFaceSetting()
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

void Post3dWindowContourGroupSettingDialog::switchFaceSetting(QListWidgetItem* current, QListWidgetItem* /*previous*/)
{
	if (current == nullptr) { return; }

	QString currentLabel = current->text();
	ui->faceSettingWidget->setSetting(m_faceMap.value(currentLabel));
	ui->faceListWidget->setCurrentItem(current, QItemSelectionModel::SelectCurrent);
}

void Post3dWindowContourGroupSettingDialog::solutionChanged(int index)
{
	auto target = m_targets.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->nodeLookupTable(target);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void Post3dWindowContourGroupSettingDialog::checkSelectedNumber()
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

void Post3dWindowContourGroupSettingDialog::updateFaceMap()
{
	m_faceMap.insert(ui->faceListWidget->currentItem()->text(), ui->faceSettingWidget->setting());
}

void Post3dWindowContourGroupSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);

	dialog.setSetting(m_scalarSetting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[target()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_scalarSetting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[target()] = dialog.title();
}

std::string Post3dWindowContourGroupSettingDialog::target() const
{
	return iRIC::toStr(ui->physicalValueComboBox->currentText());
}
