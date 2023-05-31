#include "post3dwindownodevectorarrowgroupdataitem_setfacesettingscommand.h"
#include "post3dwindownodevectorarrowgroupdataitem_settingeditwidget.h"
#include "ui_post3dwindownodevectorarrowgroupdataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowNodeVectorArrowGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_setting {item->m_setting},
	m_item {item},
	m_currentRow {-1},
	ui(new Ui::Post3dWindowNodeVectorArrowGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->faceAddButton, &QPushButton::clicked, [=](bool) {addFace();});
	connect(ui->faceRemoveButton, &QPushButton::clicked, [=](bool) {removeFace();});
	connect(ui->faceListWidget, &QListWidget::currentRowChanged, this, &SettingEditWidget::setCurrentFace);

	connect(&item->m_setting.legend.imageSetting, &ImageSettingContainer::updated, this, &SettingEditWidget::updateImageSetting);
	for (const auto& pair : item->m_colorMapSettings) {
		connect(&pair.second->legend.imageSetting, &ImageSettingContainer::updated, this, &SettingEditWidget::updateColorMapImageSetting);
	}

	auto grid = vtkStructuredGrid::SafeDownCast(m_item->data()->data()->data());
	grid->GetDimensions(m_gridDimensions);
	ui->faceSettingWidget->setDimensions(grid->GetDimensions());

	auto s = item->m_setting;
	ui->lengthSettingWidget->setSetting(s);
	ui->legendWidget->setSetting(s.legend);

	for (const auto& pair : item->m_colorMapSettings) {
		m_colorMapSettings.insert({pair.first, *pair.second});
	}

	ui->colorSettingWidget->setColorMapSettings(&m_colorMapSettings);

	m_settings = item->faceSettings();
	updateFaceList();

	if (m_settings.size() == 0) {
		clearWidgets();
	} else {
		ui->faceListWidget->setCurrentRow(0);
	}

	QList<int> sizes;
	sizes << 1 << 4 << 5;
	ui->facesSplitter->setSizes(sizes);
}

Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	saveCurrentFace();

	ui->lengthSettingWidget->updateSetting(&m_setting);
	m_setting.legend = ui->legendWidget->setting();
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget"), apply);

	command->addCommand(new ValueModifyCommmand<ArrowsSettingContainer>(iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::PropertyDialog::UpdateArrows"),
																																						true, m_setting, &m_item->m_setting));
	command->addCommand(new SetFaceSettingsCommand(m_settings, m_item));
	auto updateColormapsCommand = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::PropertyDialog::UpdateColormaps"), true);
	for (const auto& pair : m_item->m_colorMapSettings) {
		updateColormapsCommand->addCommand(new ValueModifyCommmand<ColorMapSettingContainer>(iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::PropertyDialog::UpdateColormap"),
																																												 true, m_colorMapSettings.at(pair.first), pair.second));
	}
	command->addCommand(updateColormapsCommand);

	return command;
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::addFace()
{
	ui->faceSettingWidget->setEnabled(true);

	Post3dWindowNodeVectorArrowDataItem::Setting newSetting;

	if (m_settings.size() == 0) {
		// create default
		Post3dWindowFaceSettingContainer face;
		face.enabled = true;
		face.iMin = 0;
		face.iMax = m_gridDimensions[0] - 1;
		face.jMin = 0;
		face.jMax = m_gridDimensions[1] - 1;
		face.kMin = 0;
		face.kMax = m_gridDimensions[2] - 1;
		newSetting.face = face;
	} else {
		saveCurrentFace();
		newSetting = m_settings.at(ui->faceListWidget->currentRow());
	}

	m_settings.push_back(newSetting);
	updateFaceList();

	int row = m_settings.size() - 1;
	ui->faceListWidget->setCurrentRow(row);
	ui->faceListWidget->scrollToItem(ui->faceListWidget->item(row));

	setCurrentFace(row);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::removeFace()
{
	if (m_settings.size() == 0) {return;}

	int row = ui->faceListWidget->currentRow();
	m_settings.erase(m_settings.begin() + row);
	m_currentRow = -1;

	updateFaceList();

	if (m_settings.size() == 0) {
		clearWidgets();
		return;
	} else if (row >= m_settings.size()) {
		row = m_settings.size() - 1;
	}
	ui->faceListWidget->setCurrentRow(row);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::setCurrentFace(int row)
{
	if (m_currentRow == row) {return;}

	saveCurrentFace();

	const auto& s = m_settings[row];
	ui->faceSettingWidget->setSetting(s.face);
	ui->filteringWidget->setSetting(s.filtering);
	ui->colorSettingWidget->setSetting(s.arrow);
	ui->shapeWidget->setSetting(s.arrow);

	ui->faceSettingWidget->setEnabled(true);
	ui->filteringWidget->setEnabled(true);
	ui->colorSettingWidget->setEnabled(true);
	ui->shapeWidget->setEnabled(true);

	m_currentRow = row;
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::updateImageSetting()
{
	m_setting.legend.imageSetting = m_item->m_setting.legend.imageSetting;
	ui->legendWidget->setImageSetting(m_setting.legend.imageSetting);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::updateColorMapImageSetting()
{
	auto imgSetting = dynamic_cast<ImageSettingContainer*> (sender());

	for (const auto& pair : m_item->m_colorMapSettings) {
		if (&pair.second->legend.imageSetting == imgSetting) {
			auto& cm = m_colorMapSettings.at(pair.first);
			cm.legend.imageSetting = *imgSetting;
		}
	}
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::updateFaceList()
{
	auto w = ui->faceListWidget;
	w->blockSignals(true);

	w->clear();
	int idx = 1;
	for (auto s : m_settings) {
		w->addItem(tr("Face%1").arg(idx));
		++ idx;
	}
	w->blockSignals(false);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::saveCurrentFace()
{
	if (m_currentRow == -1) {return;}

	m_settings[m_currentRow].face = ui->faceSettingWidget->setting();
	m_settings[m_currentRow].filtering = ui->filteringWidget->setting();

	ArrowsSettingContainer arrowSetting;
	arrowSetting.target = m_item->m_target.c_str();
	ui->shapeWidget->updateSetting(&arrowSetting);
	ui->colorSettingWidget->updateSetting(&arrowSetting);
	m_settings[m_currentRow].arrow = arrowSetting;
}

void Post3dWindowNodeVectorArrowGroupDataItem::SettingEditWidget::clearWidgets()
{
	Post3dWindowNodeVectorArrowDataItem::Setting dummy;
	dummy.face.iMax = m_gridDimensions[0] - 1;
	dummy.face.jMax = m_gridDimensions[1] - 1;
	dummy.face.kMax = m_gridDimensions[2] - 1;
	ui->faceSettingWidget->setSetting(dummy.face);
	ui->filteringWidget->setSetting(dummy.filtering);
	ui->colorSettingWidget->setSetting(dummy.arrow);
	ui->shapeWidget->setSetting(dummy.arrow);

	ui->faceSettingWidget->setDisabled(true);
	ui->filteringWidget->setDisabled(true);
	ui->colorSettingWidget->setDisabled(true);
	ui->shapeWidget->setDisabled(true);
}
