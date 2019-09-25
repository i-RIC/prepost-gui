#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "attributebrowserhelper.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercellcontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcellcontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroptioncell.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QToolBar>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkActorCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>

PreProcessorGridAttributeCellGroupDataItem::PreProcessorGridAttributeCellGroupDataItem(PreProcessorDataItem* p) :
	PreProcessorDataItem {tr("Cell attributes"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	setupActors();
	p->standardItem()->takeRow(m_standardItem->row());
	PreProcessorGridTypeDataItem* typeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	const QList<SolverDefinitionGridAttribute*>& conds = typeItem->gridType()->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() == SolverDefinitionGridAttribute::CellCenter) {
			// this is a cell condition.
			PreProcessorGridAttributeCellDataItem* item = new PreProcessorGridAttributeCellDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}
	const QList<SolverDefinitionGridComplexAttribute*>& compconds = typeItem->gridType()->gridComplexAttributes();
	for (auto cit = compconds.begin(); cit != compconds.end(); ++cit) {
		SolverDefinitionGridComplexAttribute* cond = *cit;
		if (cond->position() == SolverDefinitionGridComplexAttribute::CellCenter) {
			// this is a node condition.
			PreProcessorGridAttributeCellDataItem* item = new PreProcessorGridAttributeCellDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}
	m_opacity = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(PreProcessorGridAttributeCellGroupDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
}
PreProcessorGridAttributeCellGroupDataItem::~PreProcessorGridAttributeCellGroupDataItem()
{
	renderer()->RemoveActor(m_actor);
}

std::string PreProcessorGridAttributeCellGroupDataItem::target() const
{
	return m_target;
}

void PreProcessorGridAttributeCellGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void PreProcessorGridAttributeCellGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Cell Attribute Change"));
	pushRenderCommand(cmd, this, true);
}

void PreProcessorGridAttributeCellGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == nullptr) {
		// grid is not setup yet.
		return;
	}
	if (m_target == "") {
		updateVisibilityWithoutRendering();
		return;
	}
	m_actor->GetProperty()->SetOpacity(m_opacity);
	// update current active scalar

	vtkCellData* data = g->vtkGrid()->GetCellData();
	data->SetActiveScalars(m_target.c_str());
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(m_target.c_str());
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_mapper->SetScalarModeToUseCellData();
	m_mapper->SetLookupTable(stc->vtkObj());
	m_mapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_actor);

	updateVisibilityWithoutRendering();
}

void PreProcessorGridAttributeCellGroupDataItem::informDataChange(const std::string& name)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->informGridAttributeChange(name);
}


void PreProcessorGridAttributeCellGroupDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_actor);

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_mapper->SetScalarVisibility(true);
	m_actor->SetMapper(m_mapper);

	m_actor->VisibilityOff();
}

void PreProcessorGridAttributeCellGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorGridAttributeCellGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	initAttributeBrowser();
}

void PreProcessorGridAttributeCellGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	clearAttributeBrowser();
}

void PreProcessorGridAttributeCellGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridAttributeCellGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridAttributeCellGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void PreProcessorGridAttributeCellGroupDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != nullptr) {
		vtkAlgorithm* cellsAlgo = g->vtkFilteredCellsAlgorithm();
		if (cellsAlgo != nullptr) {m_mapper->SetInputConnection(cellsAlgo->GetOutputPort());}
	}
	updateActorSettings();
}

PreProcessorGridAttributeCellDataItem* PreProcessorGridAttributeCellGroupDataItem::activeChildItem()
{
	return m_nameMap.value(m_target);
}

const QList<PreProcessorGridAttributeCellDataItem*> PreProcessorGridAttributeCellGroupDataItem::conditions() const
{
	QList<PreProcessorGridAttributeCellDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		ret.append(dynamic_cast<PreProcessorGridAttributeCellDataItem*>(*it));
	}
	return ret;
}

void PreProcessorGridAttributeCellGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) {return;}
	iRICUndoStack::instance().beginMacro(QObject::tr("Object Browser Item Change"));
	GraphicsWindowDataItem::handleStandardItemChange();
	if (m_standardItem->checkState() == Qt::Checked) {
		// uncheck the node group dataitem.
		PreProcessorGridAttributeNodeGroupDataItem* nitem = dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeGroupDataItem();
		nitem->standardItem()->setCheckState(Qt::Unchecked);
	}
	iRICUndoStack::instance().endMacro();
}

void PreProcessorGridAttributeCellGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridAttributeCellGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)){
		menu->addAction(m_showAttributeBrowserAction);
	}
}

void PreProcessorGridAttributeCellGroupDataItem::initAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		pb->view()->resetForCell(true);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->resetForCell(false);
	}
}

void PreProcessorGridAttributeCellGroupDataItem::clearAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void PreProcessorGridAttributeCellGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType cellid = findCell(p, v);
	m_attributeBrowserFixed = (cellid >= 0);
	if (cellid < 0) {
		// it is not inside a cell.
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void PreProcessorGridAttributeCellGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType cellid = findCell(p, v);
	if (cellid < 0) {
		// it is not inside a cell.
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void PreProcessorGridAttributeCellGroupDataItem::updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* /*v*/)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();

	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	auto geoTopItem = gtitem->geoDataTop();
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();

	QList<PropertyBrowserAttribute> atts;
	const QList<GridAttributeContainer*> conds = grid->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		GridAttributeContainer* cond = *it;
		auto icond = dynamic_cast<GridAttributeIntegerCellContainer*>(cond);
		auto rcond = dynamic_cast<GridAttributeRealCellContainer*>(cond);
		auto ccond = dynamic_cast<GridComplexAttributeContainer*>(cond);

		if (icond != nullptr) {
			if (icond->gridAttribute()->isOption()) {
				SolverDefinitionGridAttributeIntegerOptionCell* optCond =
					dynamic_cast<SolverDefinitionGridAttributeIntegerOptionCell*>(icond->gridAttribute());
				PropertyBrowserAttribute att(icond->gridAttribute()->caption(), optCond->enumerations().value(icond->value(cellid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(icond->gridAttribute()->caption(), icond->value(cellid));
				atts.append(att);
			}
		} else if (rcond != nullptr) {
			PropertyBrowserAttribute att(rcond->gridAttribute()->caption(), rcond->value(cellid));
			atts.append(att);
		} else if (ccond != nullptr) {
			auto group = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*>
							(geoTopItem->groupDataItem(ccond->name()));
			if (group->condition()->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}

			auto v = ccond->value(cellid);
			GridComplexConditionGroup* g = group->groups().at(v - 1);
			PropertyBrowserAttribute att(group->condition()->caption(), g->caption());
			atts.append(att);
		}
	}
	QPolygonF polygon;
	vtkPoints* points = grid->vtkGrid()->GetPoints();
	vtkCell* cell = grid->vtkGrid()->GetCell(cellid);
	double* v;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		v = points->GetPoint(cell->GetPointIds()->GetId(i));
		polygon.append(QPointF(*v, *(v + 1)));
	}
	v = points->GetPoint(cell->GetPointIds()->GetId(0));
	polygon.append(QPointF(*v, *(v + 1)));

	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		Structured2DGrid* g2 = dynamic_cast<Structured2DGrid*>(gitem->grid());
		unsigned int i, j;
		g2->getCellIJIndex(cellid, &i, &j);
		pre->propertyBrowser()->view()->setCellAttributes(i, j, polygon, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pre->propertyBrowser()->view()->setCellAttributes(cellid, polygon, atts);
	}
}

vtkIdType PreProcessorGridAttributeCellGroupDataItem::findCell(const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	PreProcessorGraphicsView* v2 = dynamic_cast<PreProcessorGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();

	double point[3];
	point[0] = x; point[1] = y; point[2] = 0;
	vtkCell* hintCell = nullptr;
	double pcoords[4];
	double weights[4];
	int subid;
	vtkIdType cellid = grid->vtkGrid()->FindCell(point, hintCell, 0, 1e-4, subid, pcoords, weights);
	return cellid;
}

bool PreProcessorGridAttributeCellGroupDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorGridAttributeCellDataItem* activeItem = activeChildItem();
	if (activeItem == nullptr) {return false;}
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();
	GridAttributeContainer* cont = grid->gridAttribute(activeItem->condition()->name());
	const auto& selectWidgets = cont->dimensions()->selectWidgets();
	if (selectWidgets.size() == 0) {return false;}
	for (int i = 0; i < selectWidgets.size(); ++i) {
		GridAttributeDimensionSelectWidget* w = selectWidgets.at(i);
		QAction* action = toolbar->addWidget(w);
		action->setVisible(true);
	}
	return true;
}

void PreProcessorGridAttributeCellGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacity.load(node);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		std::string name = dynamic_cast<PreProcessorGridAttributeCellDataItem*>(*it)->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "CellAttribute", "name", name.c_str());
		if (! childNode.isNull()) {(*it)->loadFromProjectMainFile(childNode);}
	}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeCellDataItem* tmpItem = dynamic_cast<PreProcessorGridAttributeCellDataItem*>(*it);
		if (tmpItem->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setTarget(tmpItem->condition()->name());
		}
	}
}

void PreProcessorGridAttributeCellGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_opacity.save(writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("CellAttribute");
		writer.writeAttribute("name", dynamic_cast<PreProcessorGridAttributeCellDataItem*>(*it)->condition()->name().c_str());
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
