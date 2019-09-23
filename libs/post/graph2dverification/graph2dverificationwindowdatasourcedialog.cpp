#include "ui_graph2dverificationwindowdatasourcedialog.h"

#include "datamodel/graph2dverificationwindowimportdatadataitem.h"
#include "datamodel/graph2dverificationwindowimportdatagroupdataitem.h"
#include "graph2dverificationwindowdatasourcedialog.h"
#include "graph2dverificationwindowlinesettingdialog.h"
#include "graph2dverificationwindowresultsetting.h"

#include "../../../geodata/polyline/geodatapolyline.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QTextStream>
#include <QVariant.h>

Q_DECLARE_METATYPE(const GeoDataPolyLine*)   // this type must match the template of the QVariant.value method (including the const)

Graph2dVerificationWindowDataSourceDialog::Graph2dVerificationWindowDataSourceDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dVerificationWindowDataSourceDialog)
{
	ui->setupUi(this);
	connect(ui->xAxisComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeAxis(int)));

	connect(ui->pointDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changePointComboBox(int)));
	connect(ui->oneDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOneDimComboBox(int)));
	connect(ui->twoDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTwoDimComboBox(int)));
	connect(ui->threeDimDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeThreeDimComboBox(int)));

	connect(ui->oneDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeOneDimGridLocationComboBox(int)));
	connect(ui->twoDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeTwoDimGridLocationComboBox(int)));
	connect(ui->threeDimGridLocationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeThreeDimGridLocationComboBox(int)));

	connect(ui->pointDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(pointDataFocus()));
	connect(ui->oneDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(oneDimDataFocus()));
	connect(ui->twoDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(twoDimDataFocus()));
	connect(ui->threeDimDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(threeDimDataFocus()));
	connect(ui->selectedDataListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectedFocus()));

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

Graph2dVerificationWindowDataSourceDialog::~Graph2dVerificationWindowDataSourceDialog()
{
	delete ui;
}

void Graph2dVerificationWindowDataSourceDialog::pointDataFocus()
{
	ui->oneDimDataListWidget->clearSelection();
	ui->twoDimDataListWidget->clearSelection();
	ui->threeDimDataListWidget->clearSelection();
	ui->selectedDataListWidget->clearSelection();
	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::oneDimDataFocus()
{
	ui->pointDataListWidget->clearSelection();
	ui->twoDimDataListWidget->clearSelection();
	ui->threeDimDataListWidget->clearSelection();
	ui->selectedDataListWidget->clearSelection();
	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::twoDimDataFocus()
{
	ui->pointDataListWidget->clearSelection();
	ui->oneDimDataListWidget->clearSelection();
	ui->threeDimDataListWidget->clearSelection();
	ui->selectedDataListWidget->clearSelection();
	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::threeDimDataFocus()
{
	ui->pointDataListWidget->clearSelection();
	ui->oneDimDataListWidget->clearSelection();
	ui->twoDimDataListWidget->clearSelection();
	ui->selectedDataListWidget->clearSelection();
	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::selectedFocus()
{
	ui->pointDataListWidget->clearSelection();
	ui->oneDimDataListWidget->clearSelection();
	ui->twoDimDataListWidget->clearSelection();
	ui->threeDimDataListWidget->clearSelection();
	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::setSetting(const Graph2dVerificationWindowResultSetting& setting)
{
	m_setting = setting;
#if 0 || 1
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	ui->xAxisComboBox->blockSignals(true);
	for (auto it = map.begin(); it != map.end(); ++it) {
		bool dataExist = false;
		const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = it.value();
		for (auto it2 = m.begin(); it2 != m.end(); ++it2) {
			dataExist = dataExist || (it2.value().count() > 0);
		}
		if (dataExist) {
			m_xAxisModes.append(it.key());
			ui->xAxisComboBox->addItem(Graph2dVerificationWindowResultSetting::autoXAxisLabel(it.key()));
		}
	}
	ui->xAxisComboBox->blockSignals(false);
	int index = m_xAxisModes.indexOf(m_setting.xAxisMode());
	if (index != -1) {
		m_setting.setTargetDataTypeInfo(nullptr);
		ui->xAxisComboBox->setCurrentIndex(index);
		if (index == 0) {
			changeAxis(0);
		}
	}
	m_setting = setting;  // this is required for the 'Selected Data' list to be populated
	ui->polyLineComboBox->blockSignals(true);
	for (auto &line : m_setting.polyLines()) {
		const GeoDataPolyLine* pline = line;
		ui->polyLineComboBox->addItem(pline->caption(), QVariant::fromValue(pline));
	}
	if (m_setting.targetPolyLine()) {
		int n = ui->polyLineComboBox->findData(QVariant::fromValue(m_setting.targetPolyLine()));
		Q_ASSERT(n != -1);
		if (n != -1) ui->polyLineComboBox->setCurrentIndex(n);
	}
	ui->polyLineComboBox->blockSignals(false);
	if (setting.xAxisLabel().isEmpty()) {
		m_setting.setAutoXAxisLabel();
	}
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = m_setting.targetDataTypeInfo();
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& m = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& map2 = m[m_setting.xAxisMode()];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list;
	QWidget* targetWidget = nullptr;
	QWidget* targetGridLocation = nullptr;
	if (info != nullptr) {
		switch (info->dataType) {
		case Graph2dVerificationWindowResultSetting::dtBaseIterative:
			list = map2[Graph2dVerificationWindowResultSetting::dimBase];
			index = list.indexOf(info);
			ui->pointDataComboBox->setCurrentIndex(index);
			targetWidget = ui->pointDataListWidget;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
			list = map2[Graph2dVerificationWindowResultSetting::dim1D];
			index = list.indexOf(info);
			ui->oneDimDataComboBox->setCurrentIndex(index);
			targetWidget = ui->oneDimDataListWidget;
			targetGridLocation = ui->oneDimGridLocationComboBox;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
			list = map2[Graph2dVerificationWindowResultSetting::dim2D];
			index = list.indexOf(info);
			ui->twoDimDataComboBox->setCurrentIndex(index);
			targetWidget = ui->twoDimDataListWidget;
			targetGridLocation = ui->twoDimGridLocationComboBox;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
			list = map2[Graph2dVerificationWindowResultSetting::dim3D];
			index = list.indexOf(info);
			ui->threeDimDataComboBox->setCurrentIndex(index);
			targetWidget = ui->threeDimDataListWidget;
			targetGridLocation = ui->threeDimGridLocationComboBox;
			break;
		}
	}
	QListWidget* listWidget = getActiveListWidget();
	if (listWidget != nullptr) {
		updateLists(listWidget);
	}
	if (m_setting.targetDataTypeInfo() == nullptr) {
		ui->pointDataComboBox->setEnabled(true);
		ui->pointDataListWidget->setEnabled(true);
		ui->oneDimDataComboBox->setEnabled(true);
		ui->oneDimDataListWidget->setEnabled(true);
		ui->oneDimGridLocationComboBox->setEnabled(true);
		ui->twoDimDataComboBox->setEnabled(true);
		ui->twoDimDataListWidget->setEnabled(true);
		ui->twoDimGridLocationComboBox->setEnabled(true);
		ui->threeDimDataComboBox->setEnabled(true);
		ui->threeDimDataListWidget->setEnabled(true);
		ui->threeDimGridLocationComboBox->setEnabled(true);
	} else {
		ui->pointDataComboBox->setEnabled(false);
		ui->oneDimDataComboBox->setEnabled(false);
		ui->twoDimDataComboBox->setEnabled(false);
		ui->threeDimDataComboBox->setEnabled(false);
		ui->pointDataListWidget->setEnabled(false);
		ui->oneDimDataListWidget->setEnabled(false);
		ui->twoDimDataListWidget->setEnabled(false);
		ui->threeDimDataListWidget->setEnabled(false);
		ui->oneDimGridLocationComboBox->setEnabled(false);
		ui->twoDimGridLocationComboBox->setEnabled(false);
		ui->threeDimGridLocationComboBox->setEnabled(false);
		targetWidget->setEnabled(true);
		if (targetGridLocation != nullptr) { targetGridLocation->setEnabled(true); }
	}
	resize(QSize(550, 450));
#endif
}

const Graph2dVerificationWindowResultSetting& Graph2dVerificationWindowDataSourceDialog::setting()
{
	return m_setting;
}

void Graph2dVerificationWindowDataSourceDialog::changeAxis(int index)
{
#if 0 || 1
	if (m_setting.targetDataTypeInfo() != nullptr) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			int idx = m_xAxisModes.indexOf(m_setting.xAxisMode());
			ui->xAxisComboBox->blockSignals(true);
			ui->xAxisComboBox->setCurrentIndex(idx);
			ui->xAxisComboBox->blockSignals(false);
			return;
		}
	}

	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[index];
	m_setting.setXAxisMode(axis);
	m_setting.setTargetDataTypeInfo(nullptr);
	m_setting.targetDatas().clear();
	clearTargetDataTypeInfo();

	ui->selectedDataListWidget->clear();

	// update xaxis label.
	m_setting.setAutoXAxisLabel();

	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list;

	// for pointdata
	ui->pointDataComboBox->blockSignals(true);
	ui->pointDataComboBox->clear();
	list = m[Graph2dVerificationWindowResultSetting::dimBase];
	for (int i = 0; i < list.count(); ++i) {
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[i];
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
		changePointComboBox(0);
	}

	// for 1D data
	ui->oneDimDataComboBox->blockSignals(true);
	ui->oneDimDataComboBox->clear();
	list = m[Graph2dVerificationWindowResultSetting::dim1D];
	for (int i = 0; i < list.count(); ++i) {
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[i];
		ui->oneDimDataComboBox->addItem(info->zoneName.c_str());
	}
	ui->oneDimDataComboBox->blockSignals(false);
	if (list.count() == 0) {
		ui->oneDimDataLabel->hide();
		ui->oneDimDataComboBox->hide();
		ui->oneDimDataListWidget->hide();
		ui->oneDimGridLocationLabel->hide();
		ui->oneDimGridLocationComboBox->hide();
	} else if (list.count() == 1) {
		ui->oneDimDataLabel->show();
		ui->oneDimDataComboBox->hide();
		ui->oneDimDataListWidget->show();
	} else {
		ui->oneDimDataLabel->show();
		ui->oneDimDataComboBox->show();
		ui->oneDimDataListWidget->show();
	}
	ui->oneDimGridLocationComboBox->blockSignals(true);
	ui->oneDimGridLocationComboBox->clear();
	if (list.count() > 0) {
		ui->oneDimDataComboBox->setCurrentIndex(0);
		//changeOneDimComboBox(0);
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[0];
		for (auto key : info->dataNamesMap.keys()) {
			ui->oneDimGridLocationComboBox->addItem(Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(key));
		}
		if (ui->oneDimGridLocationComboBox->count() == 1) {
			ui->oneDimGridLocationLabel->hide();
			ui->oneDimGridLocationComboBox->hide();
		} else if (ui->oneDimGridLocationComboBox->count() >= 2) {
			ui->oneDimGridLocationLabel->show();
			ui->oneDimGridLocationComboBox->show();
		}
		int index = info->dataNamesMap.keys().indexOf(info->gridLocation);
		ui->oneDimGridLocationComboBox->setCurrentIndex(index);
		changeOneDimGridLocationComboBox(index);
	}
	ui->oneDimGridLocationComboBox->blockSignals(false);

	// for 2D data
	ui->twoDimDataComboBox->blockSignals(true);
	ui->twoDimDataComboBox->clear();
	list = m[Graph2dVerificationWindowResultSetting::dim2D];
	for (int i = 0; i < list.count(); ++i) {
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[i];
		ui->twoDimDataComboBox->addItem(info->zoneName.c_str());
	}
	ui->twoDimDataComboBox->blockSignals(false);
	if (list.count() == 0) {
		ui->twoDimDataLabel->hide();
		ui->twoDimDataComboBox->hide();
		ui->twoDimDataListWidget->hide();
		ui->twoDimGridLocationLabel->hide();
		ui->twoDimGridLocationComboBox->hide();
	} else if (list.count() == 1) {
		ui->twoDimDataLabel->show();
		ui->twoDimDataComboBox->hide();
		ui->twoDimDataListWidget->show();
	} else {
		ui->twoDimDataLabel->show();
		ui->twoDimDataComboBox->show();
		ui->twoDimDataListWidget->show();
	}
	ui->twoDimGridLocationComboBox->blockSignals(true);
	ui->twoDimGridLocationComboBox->clear();
	if (list.count() > 0) {
		ui->twoDimDataComboBox->setCurrentIndex(0);
		//changeTwoDimComboBox(0);
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[0];
		for (auto key : info->dataNamesMap.keys()) {
			ui->twoDimGridLocationComboBox->addItem(Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(key));
		}
		if (ui->twoDimGridLocationComboBox->count() == 1) {
			ui->twoDimGridLocationLabel->hide();
			ui->twoDimGridLocationComboBox->hide();
		} else if (ui->twoDimGridLocationComboBox->count() >= 2) {
			ui->twoDimGridLocationLabel->show();
			ui->twoDimGridLocationComboBox->show();
		}
		int index = info->dataNamesMap.keys().indexOf(info->gridLocation);
		ui->twoDimGridLocationComboBox->setCurrentIndex(index);
		changeTwoDimGridLocationComboBox(index);
	}
	ui->twoDimGridLocationComboBox->blockSignals(false);

	// for 3D data
	ui->threeDimDataComboBox->blockSignals(true);
	ui->threeDimDataComboBox->clear();
	list = m[Graph2dVerificationWindowResultSetting::dim3D];
	for (int i = 0; i < list.count(); ++i) {
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[i];
		ui->threeDimDataComboBox->addItem(info->zoneName.c_str());
	}
	ui->threeDimDataComboBox->blockSignals(false);
	if (list.count() == 0) {
		ui->threeDimDataLabel->hide();
		ui->threeDimDataComboBox->hide();
		ui->threeDimDataListWidget->hide();
		ui->threeDimGridLocationLabel->hide();
		ui->threeDimGridLocationComboBox->hide();
	} else if (list.count() == 1) {
		ui->threeDimDataLabel->show();
		ui->threeDimDataComboBox->hide();
		ui->threeDimDataListWidget->show();
	} else {
		ui->threeDimDataLabel->show();
		ui->threeDimDataComboBox->show();
		ui->threeDimDataListWidget->show();
	}
	ui->threeDimGridLocationComboBox->blockSignals(true);
	ui->threeDimGridLocationComboBox->clear();
	if (list.count() > 0) {
		ui->threeDimDataComboBox->setCurrentIndex(0);
		//changeThreeDimComboBox(0);
		Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[0];
		for (auto key : info->dataNamesMap.keys()) {
			ui->threeDimGridLocationComboBox->addItem(Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(key));
		}
		if (ui->threeDimGridLocationComboBox->count() == 1) {
			ui->threeDimGridLocationLabel->hide();
			ui->threeDimGridLocationComboBox->hide();
		} else if (ui->threeDimGridLocationComboBox->count() >= 2) {
			ui->threeDimGridLocationLabel->show();
			ui->threeDimGridLocationComboBox->show();
		}
		int index = info->dataNamesMap.keys().indexOf(info->gridLocation);
		ui->threeDimGridLocationComboBox->setCurrentIndex(index);
		changeThreeDimGridLocationComboBox(index);
	}
	ui->threeDimGridLocationComboBox->blockSignals(false);

	if (axis == Graph2dVerificationWindowResultSetting::xaPolyline && m_setting.polyLines().size() > 0) {
		ui->polyLineLabel->show();
		ui->polyLineComboBox->show();
	} else {
		ui->polyLineLabel->hide();
		ui->polyLineComboBox->hide();
		m_setting.setTargetPolyLine(nullptr);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changePointComboBox(int index)
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dimBase];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[index];

	ui->pointDataListWidget->clear();
	Q_ASSERT(info->gridLocation == GridLocationNull);
	Q_ASSERT(info->dataNamesMap.find(info->gridLocation) != info->dataNamesMap.end());
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->pointDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeOneDimComboBox(int index)
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim1D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[index];

	ui->oneDimDataListWidget->clear();
	Q_ASSERT(info->dataNamesMap.find(info->gridLocation) != info->dataNamesMap.end());
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->oneDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeTwoDimComboBox(int index)
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim2D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[index];

	ui->twoDimDataListWidget->clear();
	Q_ASSERT(info->dataNamesMap.find(info->gridLocation) != info->dataNamesMap.end());
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->twoDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeThreeDimComboBox(int index)
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim3D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[index];

	ui->threeDimDataListWidget->clear();
	Q_ASSERT(info->dataNamesMap.find(info->gridLocation) != info->dataNamesMap.end());
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->threeDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeOneDimGridLocationComboBox(int index)
{
#if 0 || 1
	Q_ASSERT(0 <= index && index < ui->oneDimGridLocationComboBox->count());
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim1D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[ui->oneDimDataComboBox->currentIndex()];

	if (ui->selectedDataListWidget->count() > 0) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			QString text = Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(info->gridLocation);
			ui->oneDimGridLocationComboBox->blockSignals(true);
			ui->oneDimGridLocationComboBox->setCurrentText(text);
			ui->oneDimGridLocationComboBox->blockSignals(false);
			return;
		} else {
			m_setting.setTargetDataTypeInfo(nullptr);
			m_setting.targetDatas().clear();
			clearTargetDataTypeInfo();
			ui->selectedDataListWidget->clear();
		}
	}

	ui->oneDimDataListWidget->clear();
	info->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(ui->oneDimGridLocationComboBox->itemText(index));
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->oneDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeTwoDimGridLocationComboBox(int index)
{
#if 0 || 1
	Q_ASSERT(0 <= index && index < ui->twoDimGridLocationComboBox->count());
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim2D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[ui->twoDimDataComboBox->currentIndex()];

	if (ui->selectedDataListWidget->count() > 0) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			QString text = Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(info->gridLocation);
			ui->twoDimGridLocationComboBox->blockSignals(true);
			ui->twoDimGridLocationComboBox->setCurrentText(text);
			ui->twoDimGridLocationComboBox->blockSignals(false);
			return;
		} else {
			m_setting.setTargetDataTypeInfo(nullptr);
			m_setting.targetDatas().clear();
			clearTargetDataTypeInfo();
			ui->selectedDataListWidget->clear();
		}
	}

	ui->twoDimDataListWidget->clear();
	info->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(ui->twoDimGridLocationComboBox->itemText(index));
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->twoDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::changeThreeDimGridLocationComboBox(int index)
{
#if 0 || 1
	Q_ASSERT(0 <= index && index < ui->threeDimGridLocationComboBox->count());
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];
	QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim3D];
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = list[ui->threeDimDataComboBox->currentIndex()];

	if (ui->selectedDataListWidget->count() > 0) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Current setting will be discarded, are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {
			QString text = Graph2dVerificationWindowResultSetting::getGridLocationStringTranslated(info->gridLocation);
			ui->threeDimGridLocationComboBox->blockSignals(true);
			ui->threeDimGridLocationComboBox->setCurrentText(text);
			ui->threeDimGridLocationComboBox->blockSignals(false);
			return;
		} else {
			m_setting.setTargetDataTypeInfo(nullptr);
			m_setting.targetDatas().clear();
			clearTargetDataTypeInfo();
			ui->selectedDataListWidget->clear();
		}
	}

	ui->threeDimDataListWidget->clear();
	info->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(ui->threeDimGridLocationComboBox->itemText(index));
	for (auto name : info->dataNamesMap[info->gridLocation]) {
		ui->threeDimDataListWidget->addItem(name);
	}
#endif
}

void Graph2dVerificationWindowDataSourceDialog::addSetting()
{
#if 0 || 1
	if (m_setting.targetDataTypeInfo() == nullptr) {
		// set targetDataTypeInfo first.
		setTargetDataTypeInfo();
	}
	QListWidget* listWidget = getActiveListWidget();
	int index = listWidget->currentRow();
	QString selected = listWidget->currentItem()->text();
	QList<Graph2dVerificationWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	Graph2dVerificationWindowResultSetting::Setting s(selected);
	datas.append(s);

	updateLists(listWidget);
	if (index >= listWidget->count()) {index = listWidget->count() - 1;}
	listWidget->setCurrentRow(index);

	// update Y axis labels.
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asRight));

	updateButtonStatus();
#endif
}

void Graph2dVerificationWindowDataSourceDialog::removeSetting()
{
	QListWidget* listWidget = getActiveListWidget();
	int row = ui->selectedDataListWidget->currentRow();
	QList<Graph2dVerificationWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	datas.removeAt(row);

	updateLists(listWidget);
	if (ui->selectedDataListWidget->count() == 0) {
		clearTargetDataTypeInfo();
	}
	if (row >= ui->selectedDataListWidget->count()) {row = ui->selectedDataListWidget->count() - 1;}
	ui->selectedDataListWidget->setCurrentRow(row);

	// update Y axis labels.
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asRight));

	updateButtonStatus();
}

void Graph2dVerificationWindowDataSourceDialog::editSetting()
{
	int row = ui->selectedDataListWidget->currentRow();
	if (row == -1) {return;}
	QList<Graph2dVerificationWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	Graph2dVerificationWindowResultSetting::Setting& setting = datas[row];
	Graph2dVerificationWindowLineSettingDialog dialog(this);
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
	m_setting.setYAxisLeftTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asLeft));
	m_setting.setYAxisRightTitle(m_setting.autoYAxisLabel(Graph2dVerificationWindowResultSetting::asRight));
}

void Graph2dVerificationWindowDataSourceDialog::updateLists(QListWidget* listWidget)
{
#if 0 || 1
	QSet<QString> addedItems;
	const QList<Graph2dVerificationWindowResultSetting::Setting>& datas = m_setting.targetDatas();
	for (auto data : datas) {
		Q_ASSERT(! addedItems.contains(data.name()));
		addedItems.insert(data.name());
	}
	// update listWidget
	listWidget->clear();
	GridLocation_t loc = m_setting.targetDataTypeInfo()->gridLocation;
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
#endif
}


void Graph2dVerificationWindowDataSourceDialog::setTargetDataTypeInfo()
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::XAxisMode axis = m_xAxisModes[ui->xAxisComboBox->currentIndex()];
	const QMap<Graph2dVerificationWindowResultSetting::XAxisMode, QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> > >& map = m_setting.dataTypeInfoMap();
	const QMap<Graph2dVerificationWindowResultSetting::DimType, QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> >& m = map[axis];

	QWidget* target = nullptr;
	QWidget* targetGridLocation = nullptr;
	if (ui->pointDataListWidget->selectedItems().count() != 0) {
		// point data selected
		QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dimBase];
		Graph2dVerificationWindowResultSetting::DataTypeInfo* type = list[ui->pointDataComboBox->currentIndex()];
		Q_ASSERT(type->gridLocation == GridLocationNull);
		m_setting.setTargetDataTypeInfo(type);
		target = ui->pointDataListWidget;
	}
	if (ui->oneDimDataListWidget->selectedItems().count() != 0) {
		// 1D data selected
		QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim1D];
		Graph2dVerificationWindowResultSetting::DataTypeInfo* type = list[ui->oneDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(
			ui->oneDimGridLocationComboBox->itemText(ui->oneDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		target = ui->oneDimDataListWidget;
		targetGridLocation = ui->oneDimGridLocationComboBox;
	}
	if (ui->twoDimDataListWidget->selectedItems().count() != 0) {
		// 2D data selected
		QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim2D];
		Graph2dVerificationWindowResultSetting::DataTypeInfo* type = list[ui->twoDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(
			ui->twoDimGridLocationComboBox->itemText(ui->twoDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		target = ui->twoDimDataListWidget;
		targetGridLocation = ui->twoDimGridLocationComboBox;
	}
	if (ui->threeDimDataListWidget->selectedItems().count() != 0) {
		// 3D data selected
		QList<Graph2dVerificationWindowResultSetting::DataTypeInfo*> list = m[Graph2dVerificationWindowResultSetting::dim3D];
		Graph2dVerificationWindowResultSetting::DataTypeInfo* type = list[ui->threeDimDataComboBox->currentIndex()];
		type->gridLocation = Graph2dVerificationWindowResultSetting::getGridLocationTranslated(
			ui->threeDimGridLocationComboBox->itemText(ui->threeDimGridLocationComboBox->currentIndex()));
		m_setting.setTargetDataTypeInfo(type);
		target = ui->threeDimDataListWidget;
		targetGridLocation = ui->threeDimGridLocationComboBox;
	}
	ui->pointDataComboBox->setEnabled(false);
	ui->pointDataListWidget->setEnabled(false);
	ui->oneDimDataComboBox->setEnabled(false);
	ui->oneDimGridLocationComboBox->setEnabled(false);
	ui->oneDimDataListWidget->setEnabled(false);
	ui->twoDimDataComboBox->setEnabled(false);
	ui->twoDimGridLocationComboBox->setEnabled(false);
	ui->twoDimDataListWidget->setEnabled(false);
	ui->threeDimDataComboBox->setEnabled(false);
	ui->threeDimGridLocationComboBox->setEnabled(false);
	ui->threeDimDataListWidget->setEnabled(false);
	if (target != nullptr) {target->setEnabled(true);}
	if (targetGridLocation != nullptr) { targetGridLocation->setEnabled(true); }
#endif
}

void Graph2dVerificationWindowDataSourceDialog::clearTargetDataTypeInfo()
{
	ui->pointDataComboBox->setEnabled(true);
	ui->pointDataListWidget->setEnabled(true);
	ui->oneDimDataComboBox->setEnabled(true);
	ui->oneDimGridLocationComboBox->setEnabled(true);
	ui->oneDimDataListWidget->setEnabled(true);
	ui->twoDimDataComboBox->setEnabled(true);
	ui->twoDimGridLocationComboBox->setEnabled(true);
	ui->twoDimDataListWidget->setEnabled(true);
	ui->threeDimDataComboBox->setEnabled(true);
	ui->threeDimGridLocationComboBox->setEnabled(true);
	ui->threeDimDataListWidget->setEnabled(true);
#if 0 || 1
	m_setting.setTargetDataTypeInfo(nullptr);
#endif
}

QListWidget* Graph2dVerificationWindowDataSourceDialog::getActiveListWidget()
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* tinfo = m_setting.targetDataTypeInfo();
	if (tinfo != nullptr) {
		switch (tinfo->dataType) {
		case Graph2dVerificationWindowResultSetting::dtBaseIterative:
			return ui->pointDataListWidget;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
			return ui->oneDimDataListWidget;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
			return ui->twoDimDataListWidget;
			break;
		case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
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
#endif
	return nullptr;
}

void Graph2dVerificationWindowDataSourceDialog::updateButtonStatus()
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

void Graph2dVerificationWindowDataSourceDialog::accept()
{
#if 0 || 1
	if (m_setting.targetDataTypeInfo() == nullptr) {
		QMessageBox::warning(this, tr("Warning"), tr("No data is selected."));
		return;
	}
#endif
	if (m_setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaPolyline) {
		const GeoDataPolyLine* pline = ui->polyLineComboBox->currentData().value<const GeoDataPolyLine*>();
		Q_ASSERT(pline != nullptr);
		m_setting.setTargetPolyLine(pline);
	}
	else {
		m_setting.setTargetPolyLine(nullptr);
	}
	m_importDataGroup->setData(m_data);
	for (int i = 0; i < m_deletedData.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* dItem = m_deletedData.at(i);
		delete dItem;
	}
	QDialog::accept();
}


void Graph2dVerificationWindowDataSourceDialog::reject()
{
	for (int i = 0; i < m_addedData.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* dItem = m_addedData.at(i);
		delete dItem;
	}
	QDialog::reject();
}

void Graph2dVerificationWindowDataSourceDialog::setImportData(Graph2dVerificationWindowImportDataGroupDataItem* group)
{
	m_importDataGroup = group;
	m_data = group->data();
	updateImportDataList();
}

void Graph2dVerificationWindowDataSourceDialog::importCsv()
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
	QList<QVector<double> > values;
	QVector<double> emptyVec;
	for (int i = 0; i < titles.count(); ++i) {
		values.append(emptyVec);
	}
	while (! csvStream.atEnd()) {
		QString line = csvStream.readLine();
		QList<QString> frags = iRIC::parseCSVLine(line);
		for (int i = 0; i < frags.count(); ++i) {
			QString f = frags[i].trimmed();
			if (! f.isEmpty()) {
				double val = f.toDouble();
				values[i].append(val);
			}
		}
	}
	csvFile.close();

	if (titles.at(0) != "X") {
		QMessageBox::critical(this, tr("Error"), tr("The title of the first column has to be \"X\""));
		return;
	}
	QVector<double> xVals;
	QVector<double> yVals;

	QFileInfo finfo(fname);
	QString shortname = finfo.fileName();
	Graph2dVerificationWindowResultSetting::Setting setting;
	for (int i = 0; i < titles.count(); ++i) {
		QString title = titles.at(i);
		if (title == "X") {
			xVals = values.at(i);
			continue;
		}
		yVals = values.at(i);
		if (xVals.count() != yVals.count()) {
			// skip this data.
			QMessageBox::warning(this, tr("Warning"), tr("The number of data of %1 and %2 mismatch. Data %1 is skipped."));
			continue;
		}
		Graph2dVerificationWindowImportDataDataItem* item = new Graph2dVerificationWindowImportDataDataItem(m_importDataGroup);
		item->setData(xVals, yVals);
		// TODO change line color.
		item->setSetting(setting);
		item->setId(shortname, title);
		m_data.append(item);
		m_addedData.append(item);
	}
	updateImportDataList();
}

void Graph2dVerificationWindowDataSourceDialog::deleteImportItem()
{
	int row = ui->importDataList->currentRow();
	if (row == -1) {return;}
	Graph2dVerificationWindowImportDataDataItem* item = m_data.at(row);
	m_data.removeAt(row);
	m_deletedData.append(item);
	QListWidgetItem* i = ui->importDataList->takeItem(row);
	delete i;
}

void Graph2dVerificationWindowDataSourceDialog::updateImportDataList()
{
	ui->importDataList->clear();
	for (int i = 0; i < m_data.count(); ++i) {
		Graph2dVerificationWindowImportDataDataItem* item = m_data.at(i);
		ui->importDataList->addItem(item->title());
	}
}
