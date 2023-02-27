#include "../post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyewindowgridshapedataitem_propertydialog.h"
#include "post2dbirdeyewindowgridshapedataitem_updateactorsettingscommand.h"
#include "ui_post2dbirdeyewindowgridshapedataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/misc/vtkpointsetandvaluerangeset.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

#include <map>

Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::PropertyDialog(Post2dBirdEyeWindowGridShapeDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	m_valueNames {},
	ui(new Ui::Post2dBirdEyeWindowGridShapeDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);

	auto grid = item->zoneDataItem()->dataContainer()->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}
	ui->shapeEditWidget->setSetting(&item->m_setting);

	std::map<QString, std::string> captionMap;
	auto gType = item->zoneDataItem()->dataContainer()->gridType();
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->GetPointData())) {
		auto cap = gType->output(name)->caption();
		captionMap.insert({cap, name});
	}
	int idx = 0;
	for (const auto& pair : captionMap) {
		ui->elevationComboBox->addItem(pair.first);
		m_valueNames.push_back(pair.second);
		if (item->m_elevationTarget == pair.second.c_str()) {
			idx = m_valueNames.size() - 1;
		}
	}
	ui->elevationComboBox->setCurrentIndex(idx);
}

Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::ModifyCommand"), true);
	command->addCommand(ui->shapeEditWidget->createModifyCommand());

	StringContainer newElev("elevationTarget");
	newElev = m_valueNames.at(ui->elevationComboBox->currentIndex()).c_str();
	command->addCommand(new ValueModifyCommmand<StringContainer>(iRIC::generateCommandId("ModifyElevationTarget"), true, newElev, &m_item->m_elevationTarget));

	return new UpdateActorSettingsCommand(apply, command, m_item);
}

void Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item);
	QDialog::accept();
}

void Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dBirdEyeWindowGridShapeDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item);
}
