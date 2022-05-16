#include "ui_graph2dhybridwindowdatasourcedialog.h"

#include "datamodel/graph2dhybridwindowimportdatadataitem.h"
#include "datamodel/graph2dhybridwindowimportdatagroupdataitem.h"
#include "graph2dhybridwindowdatasourcedialog.h"
#include "graph2dhybridwindowlinesettingdialog.h"
#include "graph2dhybridwindowresultsetting.h"

#include <geodata/polyline/geodatapolyline.h>
#include <geodata/polylinegroup/geodatapolylinegroup.h>
#include <geodata/polylinegroup/geodatapolylinegrouppolyline.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QTextStream>
#include <QVariant>

Q_DECLARE_METATYPE(const GeoDataPolyLine*)   // this type must match the template of the QVariant.value method (including the const)
Q_DECLARE_METATYPE(const GeoDataPolyLineGroup*)   // this type must match the template of the QVariant.value method (including the const)
Q_DECLARE_METATYPE(const GeoDataPolyLineGroupPolyLine*)   // this type must match the template of the QVariant.value method (including the const)

namespace {

void comboBoxSetCurrentIndexBlockingSignals(QComboBox* comboBox, int index)
{
	comboBox->blockSignals(true);
	comboBox->setCurrentIndex(index);
	comboBox->blockSignals(false);
}

void setupListWidget(QListWidget* listWidget, const QStringList& list)
{
	listWidget->clear();
	for (const auto& item : list) {
		listWidget->addItem(item);
	}
}

} // namespace

Graph2dHybridWindowDataSourceDialog::Graph2dHybridWindowDataSourceDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dHybridWindowDataSourceDialog)
{
	ui->setupUi(this);
	setupMainWidgets();

	connect(ui->xAxisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleXAxisComboBoxChange(int)));
	connect(ui->polyLineGroupComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePolyLineGroupComboBoxChange(int)));

	connect(ui->pointDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handlePointComboBoxChange(int)));
	connect(ui->oneDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleOneDimComboBoxChange(int)));
	connect(ui->twoDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTwoDimComboBoxChange(int)));
	connect(ui->threeDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleThreeDimComboBoxChange(int)));

	connect(ui->oneDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleOneDimGridLocationComboBoxChange(int)));
	connect(ui->twoDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleTwoDimGridLocationComboBoxChange(int)));
	connect(ui->threeDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleThreeDimGridLocationComboBoxChange(int)));

	connect(ui->pointDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handlePointDataListClick()));
	connect(ui->oneDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleOneDimDataListClick()));
	connect(ui->twoDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleTwoDimDataListClick()));
	connect(ui->threeDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleThreeDimDataListClick()));
	connect(ui->selectedDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(handleSelectedDataListClick()));

	connect(ui->pointDataListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSetting()));
	connect(ui->oneDimDataListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSetting()));
	connect(ui->twoDimDataListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSetting()));
	connect(ui->threeDimDataListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSetting()));
	connect(ui->selectedDataListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeSetting()));

	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addSetting()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeSetting()));
	connect(ui->settingPushButton, SIGNAL(clicked()), this, SLOT(editSetting()));

	connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importCsv()));
	connect(ui->deleteImportDataButton, SIGNAL(clicked()), this, SLOT(deleteImportItem()));
}

Graph2dHybridWindowDataSourceDialog::~Graph2dHybridWindowDataSourceDialog()
{
	delete ui;
}

void Graph2dHybridWindowDataSourceDialog::handlePointDataListClick()
{
	clearListSelectionExcept(ui->pointDataListWidget);
	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::handleOneDimDataListClick()
{
	clearListSelectionExcept(ui->oneDimDataListWidget);
	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::handleTwoDimDataListClick()
{
	clearListSelectionExcept(ui->twoDimDataListWidget);
	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::handleThreeDimDataListClick()
{
	clearListSelectionExcept(ui->threeDimDataListWidget);
	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::handleSelectedDataListClick()
{
	clearListSelectionExcept(ui->selectedDataListWidget);
	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::setSetting(const Graph2dHybridWindowResultSetting& setting)
{
	m_setting = setting;

	setupXAxisComboBox();
	setupPolyLineGroupComboBox();
	updatePolyLineComboBox();
	updateSettingWidgets();
	updatePolyLineComboBoxVisibility();

	int index = m_xAxisModes.indexOf(m_setting.xAxisMode());
	if (index != -1) {
		comboBoxSetCurrentIndexBlockingSignals(ui->xAxisComboBox, index);
	}

	if (setting.xAxisLabel().isEmpty()) {
		m_setting.setAutoXAxisLabel();
	}
	auto info = m_setting.targetDataTypeInfo();
	const auto& map = m_setting.dataTypeInfoMap();
	const auto& m = map[m_setting.xAxisMode()];

	QList<Graph2dHybridWindowResultSetting::DataTypeInfo*> list;
	QWidget* targetWidget = nullptr;
	QWidget* targetGridLocationComboBox = nullptr;

	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		updatePolyLineGroupComboBoxSelection();
		updatePolyLineComboBox();
		updatePolyLineComboBoxSelection();
	} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLine) {
		updatePolyLineComboBoxSelection();
	}
	updatePolyLineComboBoxVisibility();

	if (info != nullptr) {
		switch (info->dataType) {
		case Graph2dHybridWindowResultSetting::dtBaseIterative:
			list = m[Graph2dHybridWindowResultSetting::dimBase];
			index = list.indexOf(info);
			comboBoxSetCurrentIndexBlockingSignals(ui->pointDataComboBox, index);
			targetWidget = ui->pointDataListWidget;
			break;
		case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
			list = m[Graph2dHybridWindowResultSetting::dim1D];
			index = list.indexOf(info);
			comboBoxSetCurrentIndexBlockingSignals(ui->oneDimDataComboBox, index);
			targetWidget = ui->oneDimDataListWidget;
			targetGridLocationComboBox = ui->oneDimGridLocationComboBox;
			break;
		case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
			list = m[Graph2dHybridWindowResultSetting::dim2D];
			index = list.indexOf(info);
			comboBoxSetCurrentIndexBlockingSignals(ui->twoDimDataComboBox, index);
			targetWidget = ui->twoDimDataListWidget;
			targetGridLocationComboBox = ui->twoDimGridLocationComboBox;
			break;
		case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
			list = m[Graph2dHybridWindowResultSetting::dim3D];
			index = list.indexOf(info);
			comboBoxSetCurrentIndexBlockingSignals(ui->threeDimDataComboBox, index);
			targetWidget = ui->threeDimDataListWidget;
			targetGridLocationComboBox = ui->threeDimGridLocationComboBox;
			break;
		}
	}

	QListWidget* listWidget = getActiveListWidget();
	if (listWidget != nullptr) {
		updateLists(listWidget);
	}

	if (info == nullptr) {
		enableMainWidgets(true);
	} else {
		enableMainWidgets(false);
		targetWidget->setEnabled(true);
		if (targetGridLocationComboBox != nullptr) {targetGridLocationComboBox->setEnabled(true);}
	}
	resize(QSize(550, 450));
}

const Graph2dHybridWindowResultSetting& Graph2dHybridWindowDataSourceDialog::setting()
{
	return m_setting;
}

void Graph2dHybridWindowDataSourceDialog::handleXAxisComboBoxChange(int index)
{
	auto newAxis = m_xAxisModes[index];
	auto oldAxis = m_setting.xAxisMode();
	bool clear = false;
	if (newAxis == Graph2dHybridWindowResultSetting::xaTime) {
		clear = true;
	} else if (newAxis == Graph2dHybridWindowResultSetting::xaI || newAxis == Graph2dHybridWindowResultSetting::xaJ) {
		clear = ! (oldAxis == Graph2dHybridWindowResultSetting::xaI || oldAxis == Graph2dHybridWindowResultSetting::xaJ);
	} else if (newAxis == Graph2dHybridWindowResultSetting::xaK) {
		clear = true;
	} else if (newAxis == Graph2dHybridWindowResultSetting::xaPolyLine || Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		clear = ! (oldAxis == Graph2dHybridWindowResultSetting::xaPolyLine || oldAxis == Graph2dHybridWindowResultSetting::xaPolyLineGroup);
	}

	if (clear && m_setting.targetDataTypeInfo() != nullptr) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			int idx = m_xAxisModes.indexOf(m_setting.xAxisMode());
			comboBoxSetCurrentIndexBlockingSignals(ui->xAxisComboBox, idx);
			return;
		}
	}

	m_setting.setXAxisMode(newAxis);

	if (clear) {
		m_setting.setTargetDataTypeInfo(nullptr);
		m_setting.targetDatas().clear();

		enableMainWidgets(true);
		ui->selectedDataListWidget->clear();
		updateSettingWidgets();
	}

	// update xaxis label.
	m_setting.setAutoXAxisLabel();

	// update combo box for lines and polyline
	if (newAxis == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		auto g = m_setting.polyLineGroups().at(0);
		auto l = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (g->allData().at(0));
		m_setting.setTargetPolyLineGroupPolyLine(l);
		updatePolyLineGroupComboBoxSelection();
		updatePolyLineComboBox();
		updatePolyLineComboBoxSelection();
	} else if (newAxis == Graph2dHybridWindowResultSetting::xaPolyLine) {
		auto l = m_setting.polyLines().at(0);
		m_setting.setTargetPolyLine(l);
		updatePolyLineComboBox();
		updatePolyLineComboBoxSelection();
	}

	updatePolyLineComboBoxVisibility();
}


void Graph2dHybridWindowDataSourceDialog::handlePolyLineGroupComboBoxChange(int index)
{
	auto group = m_setting.polyLineGroups().at(index);
	auto pol = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (group->allData().at(0));
	m_setting.setTargetPolyLineGroupPolyLine(pol);

	updatePolyLineComboBox();
	updatePolyLineComboBoxSelection();
}

void Graph2dHybridWindowDataSourceDialog::handlePointComboBoxChange(int index)
{
	handleDataComboBoxChange(index, ui->pointDataComboBox, ui->pointDataListWidget, Graph2dHybridWindowResultSetting::dimBase);
}

void Graph2dHybridWindowDataSourceDialog::handleOneDimComboBoxChange(int index)
{
	handleDataComboBoxChange(index, ui->oneDimDataComboBox, ui->oneDimDataListWidget, Graph2dHybridWindowResultSetting::dim1D);
}

void Graph2dHybridWindowDataSourceDialog::handleTwoDimComboBoxChange(int index)
{
	handleDataComboBoxChange(index, ui->twoDimDataComboBox, ui->twoDimDataListWidget, Graph2dHybridWindowResultSetting::dim2D);
}

void Graph2dHybridWindowDataSourceDialog::handleThreeDimComboBoxChange(int index)
{
	handleDataComboBoxChange(index, ui->threeDimDataComboBox, ui->threeDimDataListWidget, Graph2dHybridWindowResultSetting::dim3D);
}

void Graph2dHybridWindowDataSourceDialog::handleOneDimGridLocationComboBoxChange(int index)
{
	handleLocationComboBoxChange(index, ui->oneDimGridLocationComboBox, ui->oneDimDataComboBox, ui->oneDimDataListWidget,Graph2dHybridWindowResultSetting::dim1D);
}

void Graph2dHybridWindowDataSourceDialog::handleTwoDimGridLocationComboBoxChange(int index)
{
	handleLocationComboBoxChange(index, ui->twoDimGridLocationComboBox, ui->twoDimDataComboBox, ui->twoDimDataListWidget,Graph2dHybridWindowResultSetting::dim2D);
}

void Graph2dHybridWindowDataSourceDialog::handleThreeDimGridLocationComboBoxChange(int index)
{
	handleLocationComboBoxChange(index, ui->threeDimGridLocationComboBox, ui->threeDimDataComboBox, ui->threeDimDataListWidget,Graph2dHybridWindowResultSetting::dim3D);
}

void Graph2dHybridWindowDataSourceDialog::addSetting()
{
	if (m_setting.targetDataTypeInfo() == nullptr) {
		// set targetDataTypeInfo first.
		setupTargetDataTypeInfo();
	}
	QListWidget* listWidget = getActiveListWidget();
	int index = listWidget->currentRow();
	QString selected = listWidget->currentItem()->text();
	QList<Graph2dHybridWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	Graph2dHybridWindowResultSetting::Setting s(selected);
	datas.append(s);

	updateLists(listWidget);
	if (index >= listWidget->count()) {index = listWidget->count() - 1;}
	listWidget->setCurrentRow(index);

	// update Y axis labels.
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asRight));

	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::removeSetting()
{
	QListWidget* listWidget = getActiveListWidget();
	int row = ui->selectedDataListWidget->currentRow();
	QList<Graph2dHybridWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	datas.removeAt(row);

	updateLists(listWidget);
	if (ui->selectedDataListWidget->count() == 0) {
		clearTargetDataTypeInfo();
	}
	if (row >= ui->selectedDataListWidget->count()) {row = ui->selectedDataListWidget->count() - 1;}
	ui->selectedDataListWidget->setCurrentRow(row);

	// update Y axis labels.
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asRight));

	updateButtonStatus();
}

void Graph2dHybridWindowDataSourceDialog::editSetting()
{
	int row = ui->selectedDataListWidget->currentRow();
	if (row == -1) {return;}
	QList<Graph2dHybridWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	Graph2dHybridWindowResultSetting::Setting& setting = datas[row];
	Graph2dHybridWindowLineSettingDialog dialog(this);
	dialog.setCustomColor(setting.customColor());
	dialog.setLineWidth(setting.lineWidth());
	dialog.setAxisSide(setting.axisSide());
	if (dialog.exec() == QDialog::Rejected) {
		return;
	}
	setting.setCustomColor(dialog.customColor());
	setting.setLineWidth(dialog.lineWidth());
	setting.setAxisSide(dialog.axisSide());

	// update Y axis labels.
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dHybridWindowResultSetting::asRight));
}

void Graph2dHybridWindowDataSourceDialog::updateLists(QListWidget* listWidget)
{
	QSet<QString> addedItems;
	const auto& datas = m_setting.targetDatas();
	for (auto data : datas) {
		Q_ASSERT(! addedItems.contains(data.name()));
		addedItems.insert(data.name());
	}
	// update listWidget
	listWidget->clear();
	auto loc = m_setting.targetDataTypeInfo()->gridLocation;
	Q_ASSERT(m_setting.targetDataTypeInfo()->dataNamesMap.find(loc) != m_setting.targetDataTypeInfo()->dataNamesMap.end());
	QStringList names = m_setting.targetDataTypeInfo()->dataNamesMap[loc];
	for (auto name : names) {
		if (addedItems.contains(name)) { continue; }
		listWidget->addItem(name);
	}
	// update selected
	ui->selectedDataListWidget->clear();
	for (int i = 0; i < datas.count(); ++i) {
		ui->selectedDataListWidget->addItem(datas[i].name());
	}
}

void Graph2dHybridWindowDataSourceDialog::setupXAxisComboBox()
{
	const auto& map = m_setting.dataTypeInfoMap();
	auto comboBox = ui->xAxisComboBox;
	comboBox->blockSignals(true);

	m_xAxisModes.clear();
	comboBox->clear();
	for (auto it = map.begin(); it != map.end(); ++it) {
		bool dataExist = false;
		const auto& m = it.value();
		for (auto it2 = m.begin(); it2 != m.end(); ++it2) {
			dataExist = dataExist || (it2.value().count() > 0);
		}
		if (dataExist) {
			m_xAxisModes.append(it.key());
			comboBox->addItem(Graph2dHybridWindowResultSetting::autoXAxisLabel(it.key()));
		}
	}
	comboBox->blockSignals(false);
}

void Graph2dHybridWindowDataSourceDialog::setupPolyLineGroupComboBox()
{
	ui->polyLineGroupComboBox->blockSignals(true);
	ui->polyLineGroupComboBox->clear();
	for (const GeoDataPolyLineGroup* g : m_setting.polyLineGroups()) {
		ui->polyLineGroupComboBox->addItem(g->caption(), QVariant::fromValue(g));
	}
	ui->polyLineGroupComboBox->blockSignals(false);
}

void Graph2dHybridWindowDataSourceDialog::updatePolyLineGroupComboBoxSelection()
{
	auto line = m_setting.targetPolyLineGroupPolyLine();
	if (line == nullptr) {return;}
	const GeoDataPolyLineGroup* group = dynamic_cast<GeoDataPolyLineGroup*> (line->group());
	int n = ui->polyLineGroupComboBox->findData(QVariant::fromValue(group));
	Q_ASSERT(n != -1);
	comboBoxSetCurrentIndexBlockingSignals(ui->polyLineGroupComboBox, n);
}

void Graph2dHybridWindowDataSourceDialog::updatePolyLineComboBox()
{
	auto comboBox = ui->polyLineComboBox;
	comboBox->blockSignals(true);
	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		auto line = m_setting.targetPolyLineGroupPolyLine();
		if (line != nullptr) {
			auto g = line->group();
			comboBox->clear();
			for (auto d : g->allData()) {
				const GeoDataPolyLineGroupPolyLine* l = dynamic_cast<GeoDataPolyLineGroupPolyLine*> (d);
				comboBox->addItem(l->name(), QVariant::fromValue(l));
			}
		}
	} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLine) {
		comboBox->clear();
		for (const GeoDataPolyLine* line : m_setting.polyLines()) {
			comboBox->addItem(line->caption(), QVariant::fromValue(line));
		}
	}
	comboBox->blockSignals(false);
}

void Graph2dHybridWindowDataSourceDialog::updatePolyLineComboBoxSelection()
{
	int n;
	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		const GeoDataPolyLineGroupPolyLine* line = m_setting.targetPolyLineGroupPolyLine();
		n = ui->polyLineComboBox->findData(QVariant::fromValue(line));
	} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLine) {
		const GeoDataPolyLine* line = m_setting.targetPolyLine();
		n = ui->polyLineComboBox->findData(QVariant::fromValue(line));
	}
	Q_ASSERT(n != -1);
	comboBoxSetCurrentIndexBlockingSignals(ui->polyLineComboBox, n);
}

void Graph2dHybridWindowDataSourceDialog::updateSettingWidgets()
{
	auto axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const auto& map = m_setting.dataTypeInfoMap();
	const auto& m = map[axis];

	// for pointdata
	ui->pointDataComboBox->blockSignals(true);
	ui->pointDataComboBox->clear();
	auto list = m[Graph2dHybridWindowResultSetting::dimBase];
	for (int i = 0; i < list.count(); ++i) {
		Graph2dHybridWindowResultSetting::DataTypeInfo* info = list[i];
		ui->pointDataComboBox->addItem(info->zoneName.c_str());
	}
	ui->pointDataComboBox->blockSignals(false);
	if (list.count() == 0) {
		ui->pointDataLabel->hide();
		ui->pointDataComboBox->hide();
		ui->pointDataListWidget->hide();
	} else if (list.count() == 1) {
		ui->pointDataLabel->show();
		ui->pointDataComboBox->hide();
		ui->pointDataListWidget->show();
	} else {
		ui->pointDataLabel->show();
		ui->pointDataComboBox->show();
		ui->pointDataListWidget->show();
	}
	if (list.count() > 0) {
		ui->pointDataComboBox->setCurrentIndex(0);
		handlePointComboBoxChange(0);
	}

	// for 1D data
	setupWidgetForDim(ui->oneDimDataLabel, ui->oneDimDataComboBox, ui->oneDimGridLocationLabel, ui->oneDimGridLocationComboBox, ui->oneDimDataListWidget, m[Graph2dHybridWindowResultSetting::dim1D]);

	// for 2D data
	setupWidgetForDim(ui->twoDimDataLabel, ui->twoDimDataComboBox, ui->twoDimGridLocationLabel, ui->twoDimGridLocationComboBox, ui->twoDimDataListWidget, m[Graph2dHybridWindowResultSetting::dim2D]);

	// for 3D data
	setupWidgetForDim(ui->threeDimDataLabel, ui->threeDimDataComboBox, ui->threeDimGridLocationLabel, ui->threeDimGridLocationComboBox, ui->threeDimDataListWidget, m[Graph2dHybridWindowResultSetting::dim3D]);
}

void Graph2dHybridWindowDataSourceDialog::updatePolyLineComboBoxVisibility()
{
	auto axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	if (axis == Graph2dHybridWindowResultSetting::xaPolyLine && m_setting.polyLines().size() > 0) {
		ui->polyLineGroupLabel->hide();
		ui->polyLineGroupComboBox->hide();
		ui->polyLineLabel->show();
		ui->polyLineComboBox->show();
	} else if (axis == Graph2dHybridWindowResultSetting::xaPolyLineGroup && m_setting.polyLineGroups().size() > 0) {
		ui->polyLineGroupLabel->show();
		ui->polyLineGroupComboBox->show();
		ui->polyLineLabel->show();
		ui->polyLineComboBox->show();
	} else {
		ui->polyLineGroupLabel->hide();
		ui->polyLineGroupComboBox->hide();
		ui->polyLineLabel->hide();
		ui->polyLineComboBox->hide();
	}
}

void Graph2dHybridWindowDataSourceDialog::setupMainWidgets()
{
	m_mainWidgets.push_back(ui->pointDataComboBox);
	m_mainWidgets.push_back(ui->pointDataListWidget);
	m_mainWidgets.push_back(ui->oneDimDataComboBox);
	m_mainWidgets.push_back(ui->oneDimGridLocationComboBox);
	m_mainWidgets.push_back(ui->oneDimDataListWidget);
	m_mainWidgets.push_back(ui->twoDimDataComboBox);
	m_mainWidgets.push_back(ui->twoDimGridLocationComboBox);
	m_mainWidgets.push_back(ui->twoDimDataListWidget);
	m_mainWidgets.push_back(ui->threeDimDataComboBox);
	m_mainWidgets.push_back(ui->threeDimGridLocationComboBox);
	m_mainWidgets.push_back(ui->threeDimDataListWidget);
}

void Graph2dHybridWindowDataSourceDialog::enableMainWidgets(bool enable)
{
	for (auto w : m_mainWidgets) {
		w->setEnabled(enable);
	}
}

void Graph2dHybridWindowDataSourceDialog::setupWidgetForDim(QLabel* dataLabel, QComboBox* dataComboBox, QLabel* locationLabel, QComboBox* locationComboBox, QListWidget* dataListWidget, const QList<Graph2dHybridWindowResultSetting::DataTypeInfo*>& dataList)
{
	dataComboBox->blockSignals(true);
	dataComboBox->clear();
	for (int i = 0; i < dataList.count(); ++i) {
		auto info = dataList[i];
		dataComboBox->addItem(info->zoneName.c_str());
	}
	dataComboBox->blockSignals(false);

	if (dataList.count() == 0) {
		dataLabel->hide();
		dataComboBox->hide();
		locationLabel->hide();
		locationComboBox->hide();
		dataListWidget->hide();
	} else if (dataList.count() == 1) {
		dataLabel->show();
		dataComboBox->hide();
		dataListWidget->show();
	} else {
		dataLabel->show();
		dataComboBox->show();
		dataListWidget->show();
	}

	locationComboBox->blockSignals(true);
	locationComboBox->clear();
	if (dataList.count() > 0) {
		dataComboBox->setCurrentIndex(0);
		auto info = dataList.at(0);
		for (auto key : info->dataNamesMap.keys()) {
			locationComboBox->addItem(Graph2dHybridWindowResultSetting::getGridLocationStringTranslated(key));
		}
		int index = info->dataNamesMap.keys().indexOf(info->gridLocation);
		locationComboBox->setCurrentIndex(index);
		setupListWidget(dataListWidget, info->dataNamesMap[info->gridLocation]);
	}
	locationComboBox->blockSignals(false);
}

void Graph2dHybridWindowDataSourceDialog::handleDataComboBoxChange(int index, QComboBox* comboBox, QListWidget* dataListWidget, Graph2dHybridWindowResultSetting::DimType dimType)
{
	auto axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const auto& map = m_setting.dataTypeInfoMap();
	const auto& m = map[axis];
	auto list = m[dimType];
	auto info = list[index];

	if (ui->selectedDataListWidget->count() > 0) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			QString text = info->zoneName.c_str();
			comboBox->blockSignals(true);
			comboBox->setCurrentText(text);
			comboBox->blockSignals(false);
			return;
		} else {
			m_setting.setTargetDataTypeInfo(nullptr);
			m_setting.targetDatas().clear();
			clearTargetDataTypeInfo();
			ui->selectedDataListWidget->clear();
		}
	}

	setupListWidget(dataListWidget, info->dataNamesMap[info->gridLocation]);
}

void Graph2dHybridWindowDataSourceDialog::handleLocationComboBoxChange(int index, QComboBox* locationComboBox, QComboBox* dataComboBox, QListWidget* dataListWidget, Graph2dHybridWindowResultSetting::DimType dimType)
{
	Q_ASSERT(0 <= index && index < locationComboBox->count());
	auto axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const auto& map = m_setting.dataTypeInfoMap();
	const auto& m = map[axis];
	auto list = m[dimType];
	auto info = list[dataComboBox->currentIndex()];

	if (ui->selectedDataListWidget->count() > 0) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			QString text = Graph2dHybridWindowResultSetting::getGridLocationStringTranslated(info->gridLocation);
			locationComboBox->blockSignals(true);
			locationComboBox->setCurrentText(text);
			locationComboBox->blockSignals(false);
			return;
		} else {
			m_setting.setTargetDataTypeInfo(nullptr);
			m_setting.targetDatas().clear();
			clearTargetDataTypeInfo();
			ui->selectedDataListWidget->clear();
		}
	}
	info->gridLocation = Graph2dHybridWindowResultSetting::getGridLocationTranslated(locationComboBox->itemText(index));
	setupListWidget(dataListWidget, info->dataNamesMap[info->gridLocation]);
}

void Graph2dHybridWindowDataSourceDialog::clearListSelectionExcept(QListWidget* listWidget)
{
	std::vector<QListWidget*> widgets;
	widgets.push_back(ui->pointDataListWidget);
	widgets.push_back(ui->oneDimDataListWidget);
	widgets.push_back(ui->twoDimDataListWidget);
	widgets.push_back(ui->threeDimDataListWidget);
	widgets.push_back(ui->selectedDataListWidget);

	for (auto w : widgets) {
		if (w == listWidget) {continue;}
		w->clearSelection();
	}
}

void Graph2dHybridWindowDataSourceDialog::setupTargetDataTypeInfo()
{
	auto axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const auto& map = m_setting.dataTypeInfoMap();
	const auto& m = map[axis];

	QListWidget* dataListWidget = nullptr;
	QComboBox* locationComboBox = nullptr;

	if (ui->pointDataListWidget->selectedItems().count() != 0) {
		// point data selected
		QList<Graph2dHybridWindowResultSetting::DataTypeInfo*> list = m[Graph2dHybridWindowResultSetting::dimBase];
		Graph2dHybridWindowResultSetting::DataTypeInfo* type = list[ui->pointDataComboBox->currentIndex()];
		Q_ASSERT(type->gridLocation == iRICLib::H5CgnsZone::SolutionPosition::Null);
		m_setting.setTargetDataTypeInfo(type);
		dataListWidget = ui->pointDataListWidget;
	}
	if (ui->oneDimDataListWidget->selectedItems().count() != 0) {
		// 1D data selected
		QList<Graph2dHybridWindowResultSetting::DataTypeInfo*> list = m[Graph2dHybridWindowResultSetting::dim1D];
		Graph2dHybridWindowResultSetting::DataTypeInfo* type = list[ui->oneDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dHybridWindowResultSetting::getGridLocationTranslated(
			ui->oneDimGridLocationComboBox->itemText(ui->oneDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		dataListWidget = ui->oneDimDataListWidget;
		locationComboBox = ui->oneDimGridLocationComboBox;
	}
	if (ui->twoDimDataListWidget->selectedItems().count() != 0) {
		// 2D data selected
		QList<Graph2dHybridWindowResultSetting::DataTypeInfo*> list = m[Graph2dHybridWindowResultSetting::dim2D];
		Graph2dHybridWindowResultSetting::DataTypeInfo* type = list[ui->twoDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dHybridWindowResultSetting::getGridLocationTranslated(
			ui->twoDimGridLocationComboBox->itemText(ui->twoDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		dataListWidget = ui->twoDimDataListWidget;
		locationComboBox = ui->twoDimGridLocationComboBox;
	}
	if (ui->threeDimDataListWidget->selectedItems().count() != 0) {
		// 3D data selected
		QList<Graph2dHybridWindowResultSetting::DataTypeInfo*> list = m[Graph2dHybridWindowResultSetting::dim3D];
		Graph2dHybridWindowResultSetting::DataTypeInfo* type = list[ui->threeDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dHybridWindowResultSetting::getGridLocationTranslated(
			ui->threeDimGridLocationComboBox->itemText(ui->threeDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		dataListWidget = ui->threeDimDataListWidget;
		locationComboBox = ui->threeDimGridLocationComboBox;
	}
	enableMainWidgets(false);
	if (dataListWidget != nullptr) {dataListWidget->setEnabled(true);}
	if (locationComboBox != nullptr) {locationComboBox->setEnabled(true);}
}

void Graph2dHybridWindowDataSourceDialog::clearTargetDataTypeInfo()
{
	enableMainWidgets(true);
	m_setting.setTargetDataTypeInfo(nullptr);
}

QListWidget* Graph2dHybridWindowDataSourceDialog::getActiveListWidget()
{
	Graph2dHybridWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	if (tinfo != nullptr) {
		switch (tinfo->dataType) {
		case Graph2dHybridWindowResultSetting::dtBaseIterative:
			return ui->pointDataListWidget;
			break;
		case Graph2dHybridWindowResultSetting::dtDim1DStructured:
		case Graph2dHybridWindowResultSetting::dtDim1DUnstructured:
			return ui->oneDimDataListWidget;
			break;
		case Graph2dHybridWindowResultSetting::dtDim2DStructured:
		case Graph2dHybridWindowResultSetting::dtDim2DUnstructured:
			return ui->twoDimDataListWidget;
			break;
		case Graph2dHybridWindowResultSetting::dtDim3DStructured:
		case Graph2dHybridWindowResultSetting::dtDim3DUnstructured:
			return ui->threeDimDataListWidget;
			break;
		}
	}
	if (ui->pointDataListWidget->isEnabled() && ui->pointDataListWidget->currentRow() != - 1) {
		return ui->pointDataListWidget;
	} else if (ui->oneDimDataListWidget->isEnabled() && ui->oneDimDataListWidget->currentRow() != - 1) {
		return ui->oneDimDataListWidget;
	} else if (ui->twoDimDataListWidget->isEnabled() && ui->twoDimDataListWidget->currentRow() != - 1) {
		return ui->twoDimDataListWidget;
	} else if (ui->threeDimDataListWidget->isEnabled() && ui->threeDimDataListWidget->currentRow() != - 1) {
		return ui->threeDimDataListWidget;
	}
	return nullptr;
}

void Graph2dHybridWindowDataSourceDialog::updateButtonStatus()
{
	bool addok = false;
	bool removeok = false;
	if (ui->pointDataListWidget->isEnabled() && ui->pointDataListWidget->selectedItems().count() != 0) {addok = true;}
	if (ui->oneDimDataListWidget->isEnabled() && ui->oneDimDataListWidget->selectedItems().count() != 0) {addok = true;}
	if (ui->twoDimDataListWidget->isEnabled() && ui->twoDimDataListWidget->selectedItems().count() != 0) {addok = true;}
	if (ui->threeDimDataListWidget->isEnabled() && ui->threeDimDataListWidget->selectedItems().count() != 0) {addok = true;}
	if (ui->selectedDataListWidget->selectedItems().count() != 0) {removeok = true;}

	ui->addPushButton->setEnabled(addok);
	ui->removePushButton->setEnabled(removeok);
	ui->settingPushButton->setEnabled(removeok);
}

void Graph2dHybridWindowDataSourceDialog::accept()
{
	if (m_setting.targetDataTypeInfo() == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("No data is selected."));
		return;
	}

	m_setting.setTargetPolyLineGroupPolyLine(nullptr);
	m_setting.setTargetPolyLine(nullptr);

	if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLineGroup) {
		const GeoDataPolyLineGroupPolyLine* pline = ui->polyLineComboBox->currentData().value<const GeoDataPolyLineGroupPolyLine*>();
		Q_ASSERT(pline != nullptr);
		m_setting.setTargetPolyLineGroupPolyLine(pline);
	} else if (m_setting.xAxisMode() == Graph2dHybridWindowResultSetting::xaPolyLine) {
		const GeoDataPolyLine* pline = ui->polyLineComboBox->currentData().value<const GeoDataPolyLine*>();
		Q_ASSERT(pline != nullptr);
		m_setting.setTargetPolyLine(pline);
	}
	m_importDataGroup->setData(m_data);
	for (int i = 0; i < m_deletedData.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* dItem = m_deletedData.at(i);
		delete dItem;
	}
	QDialog::accept();
}


void Graph2dHybridWindowDataSourceDialog::reject()
{
	for (int i = 0; i < m_addedData.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* dItem = m_addedData.at(i);
		delete dItem;
	}
	QDialog::reject();
}

void Graph2dHybridWindowDataSourceDialog::setImportData(Graph2dHybridWindowImportDataGroupDataItem* group)
{
	m_importDataGroup = group;
	m_data = group->data();
	updateImportDataList();
}

void Graph2dHybridWindowDataSourceDialog::importCsv()
{
	QString iodir = LastIODirectory::get();
	if (m_mainWindow->isSolverRunning()) {
		m_mainWindow->warnSolverRunning();
		return;
	}
	QString fname = QFileDialog::getOpenFileName(
										this, tr("Import CSV file"), iodir, tr("CSV file (*.csv)"));
	if (fname == "") {return;}

	QFile csvFile(fname);
	csvFile.open(QFile::ReadOnly);
	QTextStream csvStream(&csvFile);

	QList<QString> titles = iRIC::parseCSVLine(csvStream.readLine());
	QList<std::vector<double> > values;
	std::vector<double> emptyVec;
	for (int i = 0; i < titles.count(); ++i) {
		values.push_back(emptyVec);
	}
	while (! csvStream.atEnd()) {
		QString line = csvStream.readLine();
		QList<QString> frags = iRIC::parseCSVLine(line);
		for (int i = 0; i < frags.count(); ++i) {
			QString f = frags[i].trimmed();
			if (! f.isEmpty()) {
				double val = f.toDouble();
				values[i].push_back(val);
			}
		}
	}
	csvFile.close();

	if (titles.at(0) != "X") {
		QMessageBox::critical(this, tr("Error"), tr("The title of the first column has to be \"X\""));
		return;
	}
	std::vector<double> xVals;
	std::vector<double> yVals;

	QFileInfo finfo(fname);
	QString shortname = finfo.fileName();
	Graph2dHybridWindowResultSetting::Setting setting;
	for (int i = 0; i < titles.count(); ++i) {
		QString title = titles.at(i);
		if (title == "X") {
			xVals = values.at(i);
			continue;
		}
		yVals = values.at(i);
		if (xVals.size() != yVals.size()) {
			// skip this data.
			QMessageBox::warning(this, tr("Warning"), tr("The number of data of %1 and %2 mismatch. Data %1 is skipped."));
			continue;
		}
		Graph2dHybridWindowImportDataDataItem* item = new Graph2dHybridWindowImportDataDataItem(m_importDataGroup);
		item->setData(xVals, yVals);
		// TODO change line color.
		item->setSetting(setting);
		item->setId(shortname, title);
		m_data.append(item);
		m_addedData.append(item);
	}
	updateImportDataList();
}

void Graph2dHybridWindowDataSourceDialog::deleteImportItem()
{
	int row = ui->importDataList->currentRow();
	if (row == -1) {return;}
	Graph2dHybridWindowImportDataDataItem* item = m_data.at(row);
	m_data.removeAt(row);
	m_deletedData.append(item);
	QListWidgetItem* i = ui->importDataList->takeItem(row);
	delete i;
}

void Graph2dHybridWindowDataSourceDialog::updateImportDataList()
{
	ui->importDataList->clear();
	for (int i = 0; i < m_data.count(); ++i) {
		Graph2dHybridWindowImportDataDataItem* item = m_data.at(i);
		ui->importDataList->addItem(item->title());
	}
}
