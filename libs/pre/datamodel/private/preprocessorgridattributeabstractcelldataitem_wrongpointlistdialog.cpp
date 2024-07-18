#include "../../preprocessorgraphicsview.h"
#include "../preprocessorgridattributeabstractcellgroupdataitem.h"
#include "../public/preprocessorgriddataitem_selecteddatawithidcontroller.h"
#include "preprocessorgridattributeabstractcelldataitem_wrongpointlistdialog.h"
#include "ui_preprocessorgridattributeabstractcelldataitem_wrongpointlistdialog.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtktool/vtkpointsutil.h>

#include <vtkStructuredGrid.h>

PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::WrongPointListDialog(PreProcessorGridAttributeAbstractCellDataItem* item, vtkIdType ds, QWidget *parent) :
	QDialog(parent),
	m_downstream {ds},
	m_item {item},
	ui(new Ui::PreProcessorGridAttributeAbstractCellDataItem_WrongPointListDialog)
{
	ui->setupUi(this);
	connect(ui->recheckButton, &QPushButton::clicked, this, &WrongPointListDialog::recheck);
	connect(ui->listWidget, &QListWidget::clicked, this, &WrongPointListDialog::handleSelectChange);
}

PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::~WrongPointListDialog()
{
	delete ui;
}

void PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::setPointList(const std::vector<vtkIdType>& list)
{
	m_list = list;

	ui->listWidget->clear();
	auto data = m_item->groupDataItem()->data()->data();
	auto grid = vtkStructuredGrid::SafeDownCast(data);
	int dims[3];
	grid->GetDimensions(dims);
	vtkIdType cellINum = dims[0] - 1;
	for (vtkIdType index : list) {
		auto i = index % cellINum;
		auto j = index / cellINum;
		ui->listWidget->addItem(QString("(%1, %2)").arg(i).arg(j));
	}
}

void PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::accept()
{
	m_item->m_wrongPoints.clear();
	m_item->updateActorSetting();
	m_item->renderGraphicsView();
}

void PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::recheck()
{
	auto data = m_item->groupDataItem()->data()->data();
	data->GetCellData()->SetActiveScalars(m_item->m_condition->name().c_str());

	auto list = m_item->m_directionSetting.findWrongPoints(data, m_downstream);
	setPointList(list);

	m_item->m_wrongPoints = list;
	m_item->updateActorSetting();
	m_item->renderGraphicsView();
}

void PreProcessorGridAttributeAbstractCellDataItem::WrongPointListDialog::handleSelectChange()
{
	auto sModel = ui->listWidget->selectionModel();
	if (sModel->selectedRows().size() == 0) {return;}

	auto row = sModel->selectedRows().at(0).row();
	auto index = m_list.at(row);

	std::vector<vtkIdType> ids;
	ids.push_back(index);
	m_item->selectedDataController()->setSelectedDataIds(ids);

	auto data = m_item->groupDataItem()->data()->data();
	auto cell = data->GetCell(index);
	auto center = vtkPointsUtil::getCenter(cell);

	auto view = dynamic_cast<PreProcessorGraphicsView*>(m_item->dataModel()->graphicsView());
	view->panTo(center.x(), center.y());
	m_item->gridDataItem()->doViewOperationEndedGlobal(view);
}
