#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.h"
#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"
#include "ui_post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/region/region2dsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::PropertyDialog(Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	m_additionalWidgets {new AdditionalWidgets(this)},
	ui(new Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog)
{
	ui->setupUi(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_additionalWidgets);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	auto dataContainer = item->zoneDataItem()->dataContainer();
	auto data = dataContainer->data()->data();
	if (! dataContainer->IBCExists()) {
		m_additionalWidgets->regionWidget()->disableActive();
	}
	m_additionalWidgets->regionWidget()->hideCustom();

	std::map<std::string, QString> names;
	auto pd = data->GetPointData();
	auto tItem = item->zoneDataItem()->gridTypeDataItem();
	auto gridType = tItem->gridType();
	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd)) {
		names.insert({name, gridType->solutionCaption(name)});
	}
	ui->arrowsSettingWidget->setValueNames(names);
	ui->arrowsSettingWidget->setColorMapSettings(m_item->m_colorMapSettings);

	ui->arrowsSettingWidget->setSetting(&item->m_setting);
	m_additionalWidgets->samplingWidget()->setSetting(&item->m_filteringSetting);
	m_additionalWidgets->regionWidget()->setSetting(&item->m_regionSetting);
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Property"), apply);
	command->addCommand(ui->arrowsSettingWidget->createModifyCommand());
	command->addCommand(m_additionalWidgets->samplingWidget()->createModifyCommand());
	command->addCommand(m_additionalWidgets->regionWidget()->createModifyCommand());

	return new UpdateActorSettingCommand(apply, command, m_item);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
}
