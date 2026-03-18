#include "../preprocessorgriddataitem.h"
#include "../preprocessorgridtypedataitem.h"
#include "preprocessorstructured2dgridshapedataitem_clipdialog.h"
#include "ui_preprocessorstructured2dgridshapedataitem_clipdialog.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/iricundostack.h>

PreProcessorStructured2dGridShapeDataItem::ClipDialog::ClipDialog(PreProcessorStructured2dGridShapeDataItem* dataItem, QWidget *parent) :
	QDialog(parent),
	m_dataItem {dataItem},
	ui(new Ui::PreProcessorStructured2dGridShapeDataItem_ClipDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	auto grid = dynamic_cast<v4Structured2dGrid*> (dataItem->gridDataItem()->grid()->grid());
	vtkIdType dimI, dimJ;
	grid->getDimensions(&dimI, &dimJ);
	ui->widget->setDimensions(dimI, dimJ);

	RegionRange2dSettingContainer setting;
	setting.iMin = 0;
	setting.iMax = dimI;
	setting.jMin = 0;
	setting.jMax = dimJ;
	ui->widget->setSetting(setting);
}

PreProcessorStructured2dGridShapeDataItem::ClipDialog::~ClipDialog()
{
	delete ui;
}

void PreProcessorStructured2dGridShapeDataItem::ClipDialog::accept()
{
	auto grid = dynamic_cast<v4Structured2dGrid*> (m_dataItem->gridDataItem()->grid()->grid());
	auto s = ui->widget->setting();

	auto newGrid = new v4Structured2dGrid();
	newGrid->setDimensions(s.iMax - s.iMin + 1, s.jMax - s.jMin + 1);

	// copy data at nodes and cells
	auto clippedData = grid->regionFilteredNodeData(s.iMin, s.iMax, s.jMin, s.jMax);
	newGrid->vtkConcreteData()->data()->DeepCopy(clippedData);
	clippedData->Delete();

	// copy data at iedge
	auto clippedIEdgeData = grid->regionFilteredIEdgeData(s.iMin, s.iMax, s.jMin, s.jMax - 1);
	newGrid->vtkIEdgeData()->concreteData()->GetCellData()->DeepCopy(clippedIEdgeData->GetCellData());
	clippedIEdgeData->Delete();

	// copy data at jedge
	auto clippedJEdgeData = grid->regionFilteredJEdgeData(s.iMin, s.iMax - 1, s.jMin, s.jMax);
	newGrid->vtkJEdgeData()->concreteData()->GetCellData()->DeepCopy(clippedJEdgeData->GetCellData());

	auto inputGrid = new v4InputGrid(m_dataItem->gridDataItem()->gridTypeDataItem()->gridType(), newGrid);
	m_dataItem->gridDataItem()->setGrid(inputGrid, false);
	m_dataItem->renderGraphicsView();

	iRICUndoStack::instance().clear();
	QDialog::accept();
}
