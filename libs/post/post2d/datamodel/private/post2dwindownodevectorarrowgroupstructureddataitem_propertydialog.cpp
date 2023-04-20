#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.h"
#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"
#include "ui_post2dwindownodevectorarrowgroupstructureddataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/structured2dfilteringsettingeditwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/region/region2dsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::PropertyDialog(Post2dWindowNodeVectorArrowGroupStructuredDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	m_additionalWidgets {new AdditionalWidgets(this)},
	ui(new Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog)
{
	ui->setupUi(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_additionalWidgets);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	auto dataContainer = item->zoneDataItem()->dataContainer();
	auto data = dataContainer->data()->data();
	if (! dataContainer->IBCExists()) {
		m_additionalWidgets->regionWidget()->disableActive();
	}
	auto structured = vtkStructuredGrid::SafeDownCast(data);
	int dims[3];
	structured->GetDimensions(dims);
	m_additionalWidgets->regionWidget()->setDimensions(dims[0], dims[1]);

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

Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupStructuredDataItem::Property"), apply);
	command->addCommand(ui->arrowsSettingWidget->createModifyCommand());
	command->addCommand(m_additionalWidgets->samplingWidget()->createModifyCommand());
	command->addCommand(m_additionalWidgets->regionWidget()->createModifyCommand());

	return new UpdateActorSettingCommand(apply, command, m_item);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
}
