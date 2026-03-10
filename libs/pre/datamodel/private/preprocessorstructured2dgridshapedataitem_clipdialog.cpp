#include "../preprocessorgriddataitem.h"
#include "preprocessorstructured2dgridshapedataitem_clipdialog.h"
#include "ui_preprocessorstructured2dgridshapedataitem_clipdialog.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/region/region2dsettingcontainer.h>

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
