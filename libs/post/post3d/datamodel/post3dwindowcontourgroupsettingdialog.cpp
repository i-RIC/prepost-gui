#include "ui_post3dwindowcontourgroupsettingdialog.h"

#include "post3dwindowcontourgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"

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
	ui->contourWidget->hidePoints();

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

void Post3dWindowContourGroupSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	ui->faceSettingWidget->setZoneData(zoneData);

	vtkPointData* pd = zoneData->data()->GetPointData();
	int num = pd->GetNumberOfArrays();
	ui->physicalValueComboBox->clear();
	m_solutions.clear();
	ui->physicalValueComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i) {
		vtkDataArray* a = pd->GetArray(i);
		if (a == nullptr) {continue;}
		if (a->GetNumberOfComponents() != 1) {continue;}
		std::string name = a->GetName();
		ui->physicalValueComboBox->addItem(name.c_str());
		m_solutions.append(name);
	}
	ui->physicalValueComboBox->blockSignals(false);
}

void Post3dWindowContourGroupSettingDialog::setCurrentSolution(const std::string& sol)
{
	int index = m_solutions.indexOf(sol);
	ui->physicalValueComboBox->setCurrentIndex(index);
}

void Post3dWindowContourGroupSettingDialog::setContour(ContourSettingWidget::Contour c)
{
	ui->contourWidget->setContour(c);
}

void Post3dWindowContourGroupSettingDialog::setNumberOfDivision(int div)
{
	ui->colormapWidget->setDivisionNumber(div);
}

void Post3dWindowContourGroupSettingDialog::setLookupTable(const LookupTableContainer& c)
{
	m_lookupTable = c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void Post3dWindowContourGroupSettingDialog::setGridTypeDataItem(Post3dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

std::string Post3dWindowContourGroupSettingDialog::currentSolution() const
{
	return iRIC::toStr(ui->physicalValueComboBox->currentText());
}

ContourSettingWidget::Contour Post3dWindowContourGroupSettingDialog::contour()
{
	return ui->contourWidget->contour();
}

int Post3dWindowContourGroupSettingDialog::numberOfDivision()
{
	return ui->colormapWidget->divisionNumber();
}

LookupTableContainer& Post3dWindowContourGroupSettingDialog::lookupTable()
{
	return m_lookupTable;
}

void Post3dWindowContourGroupSettingDialog::solutionChanged(int index)
{
	auto sol = m_solutions.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->lookupTable(sol);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void Post3dWindowContourGroupSettingDialog::accept()
{
	ui->colormapWidget->save();
	QDialog::accept();
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

const QMap<QString, Post3dWindowFaceDataItem::Setting>& Post3dWindowContourGroupSettingDialog::faceMap()
{
	return m_faceMap;
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

void Post3dWindowContourGroupSettingDialog::setFillUpper(bool fill)
{
	ui->colormapWidget->setFillUpper(fill);
}

void Post3dWindowContourGroupSettingDialog::setFillLower(bool fill)
{
	ui->colormapWidget->setFillLower(fill);
}

bool Post3dWindowContourGroupSettingDialog::fillUpper()
{
	return ui->colormapWidget->fillUpper();
}

bool Post3dWindowContourGroupSettingDialog::fillLower()
{
	return ui->colormapWidget->fillLower();
}

void Post3dWindowContourGroupSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titleMap)
{
	m_colorBarTitleMap = titleMap;
}

QString Post3dWindowContourGroupSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[currentSolution()];
}

void Post3dWindowContourGroupSettingDialog::setScalarBarSetting(const ScalarBarSetting& setting)
{
	m_scalarBarSetting = setting;
}

void Post3dWindowContourGroupSettingDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void Post3dWindowContourGroupSettingDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
}

void Post3dWindowContourGroupSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setTitle(m_colorBarTitleMap[currentSolution()]);
	dialog.setSetting(m_scalarBarSetting);
	dialog.setTitleTextSetting(m_titleTextSetting);
	dialog.setLabelTextSetting(m_labelTextSetting);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_colorBarTitleMap[currentSolution()] = dialog.title();
	m_scalarBarSetting = dialog.setting();
	m_titleTextSetting = dialog.titleTextSetting();
	m_labelTextSetting = dialog.labelTextSetting();
}
