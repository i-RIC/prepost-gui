#include "../measureddatafiledataitem.h"
#include "../measureddatapointgroupdataitem.h"
#include "../measureddatavectorsetting.h"
#include "measureddatavectorgroupdataitem_impl.h"
#include "measureddatavectorgroupdataitem_propertydialog.h"
#include "measureddatavectorgroupdataitem_updatesettingcommand.h"
#include "ui_measureddatavectorgroupdataitem_propertydialog.h"

#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/iricundostack.h>

#include <QAbstractButton>

MeasuredDataVectorGroupDataItem::PropertyDialog::PropertyDialog(MeasuredDataVectorGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_filteringEditWidget {nullptr},
	m_item {item},
	ui(new Ui::MeasuredDataVectorGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	m_filteringEditWidget = new GeneralFilteringSettingEditWidget(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_filteringEditWidget);

	auto fileDataItem = dynamic_cast<MeasuredDataFileDataItem*> (item->parent());
	auto md = fileDataItem->measuredData();

	std::map<std::string, QString> names;
	for (const auto& name : md->vectorNames()) {
		names.insert({name, name.c_str()});
	}
	ui->arrowsSettingWidget->setValueNames(names);
	ui->arrowsSettingWidget->setColorMapSettings(fileDataItem->pointGroupDataItem()->colorMapSettings());

	ui->arrowsSettingWidget->setSetting(&m_item->impl->m_setting.arrowsSetting);
	m_filteringEditWidget->setSetting(&m_item->impl->m_setting.filteringSetting);
}

MeasuredDataVectorGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* MeasuredDataVectorGroupDataItem::PropertyDialog::createModifyCommand()
{
	return new UpdateSettingCommand(
				ui->arrowsSettingWidget->createModifyCommand(),
				m_filteringEditWidget->createModifyCommand(),
				m_item);
}

void MeasuredDataVectorGroupDataItem::PropertyDialog::accept()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::accept();
}

void MeasuredDataVectorGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_item->renderGraphicsView();
	}

	QDialog::reject();
}

void MeasuredDataVectorGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void MeasuredDataVectorGroupDataItem::PropertyDialog::apply()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	m_item->pushRenderRedoOnlyCommand(createModifyCommand(), m_item, true);
	m_applied = true;
}
