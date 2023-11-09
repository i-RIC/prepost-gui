#include "../post2dwindowgridattributenodegroupdataitem.h"
#include "post2dwindowgridattributenodedataitem_propertydialog.h"
#include "ui_post2dwindowgridattributenodedataitem_propertydialog.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowGridAttributeNodeDataItem::PropertyDialog::PropertyDialog(Post2dWindowGridAttributeNodeGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowGridAttributeNodeDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
}

Post2dWindowGridAttributeNodeDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

ColorMapSettingEditWidgetI* Post2dWindowGridAttributeNodeDataItem::PropertyDialog::widget() const
{
	return dynamic_cast<ColorMapSettingEditWidgetI*> (ui->widgetContainer->widget());
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::setWidget(ColorMapSettingEditWidgetI* w)
{
	ui->widgetContainer->setWidget(w);
	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}

OpacityContainer Post2dWindowGridAttributeNodeDataItem::PropertyDialog::opacity() const
{
	return ui->transparencyWidget->opacity();
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::setOpacity(const OpacityContainer& opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::accept()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(false), m_item);
	QDialog::accept();
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dWindowGridAttributeNodeDataItem::PropertyDialog::apply()
{
	m_item->pushUpdateActorSettingCommand(createModifyCommand(true), m_item);
	m_applied = true;
}

QUndoCommand* Post2dWindowGridAttributeNodeDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowGridAttributeAbstractCellDataItem::PropertyDialog"), apply);
	ret->addCommand(widget()->createModifyCommand());
	ret->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("Opacity"), apply, opacity(), &m_item->opacity()));

	return ret;
}
