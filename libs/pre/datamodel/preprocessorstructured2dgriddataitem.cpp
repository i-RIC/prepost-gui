#include "preprocessorbcgroupdataitem.h"
#include "preprocessorbcgroupdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorstructured2dgriddataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"

#include <guibase/vtkgridedgeutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include <QAction>
#include <QMenu>

PreProcessorStructured2dGridDataItem::PreProcessorStructured2dGridDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridDataItem {parent},
	m_edgeMapper {vtkPolyDataMapper::New()},
	m_edgeActor {vtkActor::New()},
	m_selectMenu {nullptr}
{
	m_shapeDataItem = new PreProcessorStructured2dGridShapeDataItem(this);
	m_childItems.push_back(m_shapeDataItem);

	m_regionSelectAction = new QAction(PreProcessorStructured2dGridDataItem::tr("Select I-J &Region..."), this);
	m_regionSelectAction->setDisabled(true);
	setupMenu();
	updateObjectBrowserTree();

	vtkGridEdgeUtil::setupActor(m_edgeActor);
	m_edgeActor->SetMapper(m_edgeMapper);
	m_edgeActor->VisibilityOff();
	renderer()->AddActor(m_edgeActor);
}

PreProcessorStructured2dGridDataItem::~PreProcessorStructured2dGridDataItem()
{
	renderer()->RemoveActor(m_edgeActor);

	m_edgeMapper->Delete();
	m_edgeActor->Delete();
}

void PreProcessorStructured2dGridDataItem::setEdgeFocus(vtkIdType i, vtkIdType j)
{
	auto g1 = grid();
	if (g1 == nullptr) {return;}
	auto g2 = dynamic_cast<v4Structured2dGrid*> (g1->grid());

	auto polyData = g2->extractEdgeData(i, j);
	m_edgeMapper->SetInputData(polyData);
	polyData->Delete();
	m_edgeActor->VisibilityOn();
}
void PreProcessorStructured2dGridDataItem::clearEdgeFocus()
{
	m_edgeActor->VisibilityOff();
}

void PreProcessorStructured2dGridDataItem::setupMenu()
{
	m_editMenu = menu()->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_shapeDataItem->editAction());
	m_editMenu->addAction(nodeEditAction());
	m_editMenu->addAction(cellEditAction());

	if (m_bcGroupDataItem != nullptr) {
		m_editMenu->addMenu(m_bcGroupDataItem->bcMenu());
	}
	menu()->addAction(deleteAction());
	menu()->addSeparator();

	m_displayMenu = menu()->addMenu(tr("Dis&play Setting..."));
	m_displayMenu->addAction(displaySettingAction());
	m_displayMenu->addAction(nodeDisplaySettingAction());
	m_displayMenu->addAction(cellDisplaySettingAction());
}

void PreProcessorStructured2dGridDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_edgeActor->SetPosition(0, 0, range.max());

	PreProcessorGridDataItem::assignActorZValues(range);
}

void PreProcessorStructured2dGridDataItem::updateActionStatus()
{
	m_editMenu->setEnabled(grid() != nullptr);
	m_displayMenu->setEnabled(grid() != nullptr);

	PreProcessorGridDataItem::updateActionStatus();
}
