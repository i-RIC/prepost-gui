#include "preprocessorgridrelatedconditioncellgroupdataitem.h"
#include "preprocessorgridrelatedconditioncelldataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgriddataitem.h"
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcomplexcondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include "../preprocessordatamodel.h"
#include <misc/iricundostack.h>
#include "preprocessorgridrelatedconditionnodegroupdataitem.h"
#include <guicore/datamodel/propertybrowserview.h>
#include "../preprocessorgraphicsview.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionintegercellcontainer.h>
#include <guicore/pre/gridcond/container/gridrelatedconditionrealcellcontainer.h>
#include <guicore/solverdef/solverdefinitionintegeroptioncellgridrelatedcondition.h>
#include <guicore/base/propertybrowser.h>
#include "../preprocessorwindow.h"

#include <QList>
#include <QStandardItem>
#include <QXmlStreamWriter>
#include <QDomNode>
#include <QUndoCommand>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QToolBar>

#include <vtkActorCollection.h>
#include <vtkLODActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkContourFilter.h>
#include <vtkCellData.h>
#include <vtkAppendPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkDoubleArray.h>
#include <vtkCellData.h>
#include <vtkDelaunay2D.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkCell.h>

PreProcessorGridRelatedConditionCellGroupDataItem::PreProcessorGridRelatedConditionCellGroupDataItem(PreProcessorDataItem* p)
	: PreProcessorDataItem(tr("Cell attributes"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
	p->standardItem()->takeRow(m_standardItem->row());
	PreProcessorGridTypeDataItem* typeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	const QList<SolverDefinitionGridRelatedCondition*>& conds = typeItem->gridType()->gridRelatedConditions();
	for (auto it = conds.begin(); it != conds.end(); ++it){
		SolverDefinitionGridRelatedCondition* cond = *it;
		if (cond->position() == SolverDefinitionGridRelatedCondition::CellCenter){
			// this is a cell condition.
			PreProcessorGridRelatedConditionCellDataItem* item = new PreProcessorGridRelatedConditionCellDataItem(cond, this);
			m_childItems.append(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}
	const QList<SolverDefinitionGridRelatedComplexCondition*>& compconds = typeItem->gridType()->gridRelatedComplexConditions();
	for (auto cit = compconds.begin(); cit != compconds.end(); ++cit){
		SolverDefinitionGridRelatedComplexCondition* cond = *cit;
		if (cond->position() == SolverDefinitionGridRelatedComplexCondition::CellCenter){
			// this is a node condition.
			PreProcessorGridRelatedConditionCellDataItem* item = new PreProcessorGridRelatedConditionCellDataItem(cond, this);
			m_childItems.append(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}
	m_opacityPercent = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
}
PreProcessorGridRelatedConditionCellGroupDataItem::~PreProcessorGridRelatedConditionCellGroupDataItem()
{
	renderer()->RemoveActor(m_actor);
}

class PreProcessorGridRelatedConditionCellGroupSelectCondition : public QUndoCommand
{
public:
	PreProcessorGridRelatedConditionCellGroupSelectCondition(const QString& newcond, PreProcessorGridRelatedConditionCellGroupDataItem* item)
		: QUndoCommand(PreProcessorGridRelatedConditionCellGroupDataItem::tr("Cell Attribute Change"))
	{
		m_newCurrentCondition = newcond;
		m_oldCurrentCondition = item->m_currentCondition;
		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentCondition(m_oldCurrentCondition);
		m_item->updateActorSettings();
		PreProcessorGridDataItem* gItem =
				dynamic_cast<PreProcessorGridDataItem*>(m_item->parent());
		gItem->updateSimplefiedGrid();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentCondition(m_newCurrentCondition);
		m_item->updateActorSettings();
		PreProcessorGridDataItem* gItem =
				dynamic_cast<PreProcessorGridDataItem*>(m_item->parent());
		gItem->updateSimplefiedGrid();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_oldCurrentCondition;
	QString m_newCurrentCondition;

	PreProcessorGridRelatedConditionCellGroupDataItem* m_item;
};

void PreProcessorGridRelatedConditionCellGroupDataItem::exclusivelyCheck(PreProcessorGridRelatedConditionCellDataItem* item)
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked){
		stack.push(new PreProcessorGridRelatedConditionCellGroupSelectCondition("", this));
	}else{
		stack.push(new PreProcessorGridRelatedConditionCellGroupSelectCondition(item->condition()->name(), this));
	}
}

void PreProcessorGridRelatedConditionCellGroupDataItem::setCurrentCondition(const QString& currentCond)
{
	PreProcessorGridRelatedConditionCellDataItem* current = 0;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorGridRelatedConditionCellDataItem* tmpItem = dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(*it);
		if (tmpItem->condition()->name() == currentCond){
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != 0){
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_currentCondition = currentCond;
}

void PreProcessorGridRelatedConditionCellGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == 0){
		// grid is not setup yet.
		return;
	}
	if (m_currentCondition == ""){
		updateVisibilityWithoutRendering();
		return;
	}
	m_actor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
	// update current active scalar

	vtkCellData* data = g->vtkGrid()->GetCellData();
	data->SetActiveScalars(iRIC::toStr(m_currentCondition).c_str());
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(iRIC::toStr(m_currentCondition).c_str());
	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	m_mapper->SetScalarModeToUseCellData();
	m_mapper->SetLookupTable(stc->vtkObj());
	m_mapper->UseLookupTableScalarRangeOn();
	m_actorCollection->AddItem(m_actor);

	updateVisibilityWithoutRendering();
}

void PreProcessorGridRelatedConditionCellGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacityPercent = loadOpacityPercent(node);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		QString name = dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(*it)->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "CellAttribute", "name", name);
		if (! childNode.isNull()){(*it)->loadFromProjectMainFile(childNode);}
	}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		PreProcessorGridRelatedConditionCellDataItem* tmpItem = dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(*it);
		if (tmpItem->standardItem()->checkState() == Qt::Checked){
			// this is the current Condition!
			setCurrentCondition(tmpItem->condition()->name());
		}
	}
}

void PreProcessorGridRelatedConditionCellGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writeOpacityPercent(m_opacityPercent, writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		writer.writeStartElement("CellAttribute");
		writer.writeAttribute("name", dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(*it)->condition()->name());
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridRelatedConditionCellGroupDataItem::informDataChange(const QString& name)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->informgridRelatedConditionChange(name);
}


void PreProcessorGridRelatedConditionCellGroupDataItem::setupActors(){
	m_actor = vtkSmartPointer<vtkLODActor>::New();
	m_actor->SetNumberOfCloudPoints(0);
	m_actor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_actor);

//	m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_mapper->SetScalarVisibility(true);
	m_actor->SetMapper(m_mapper);

	m_actor->VisibilityOff();
}

void PreProcessorGridRelatedConditionCellGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::informSelection(VTKGraphicsView * /*v*/)
{
	initAttributeBrowser();
}

void PreProcessorGridRelatedConditionCellGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	clearAttributeBrowser();
}

void PreProcessorGridRelatedConditionCellGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void PreProcessorGridRelatedConditionCellGroupDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != 0){
		vtkAlgorithm* cellsAlgo = g->vtkFilteredCellsAlgorithm();
		if (cellsAlgo != 0){m_mapper->SetInputConnection(cellsAlgo->GetOutputPort());}
//		m_mapper->SetInputData(g->vtkGrid());
	}
	updateActorSettings();
}

PreProcessorGridRelatedConditionCellDataItem* PreProcessorGridRelatedConditionCellGroupDataItem::activeChildItem()
{
	return m_nameMap.value(m_currentCondition);
}

const QList<PreProcessorGridRelatedConditionCellDataItem*> PreProcessorGridRelatedConditionCellGroupDataItem::conditions() const
{
	QList<PreProcessorGridRelatedConditionCellDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it){
		ret.append(dynamic_cast<PreProcessorGridRelatedConditionCellDataItem*>(*it));
	}
	return ret;
}

void PreProcessorGridRelatedConditionCellGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting){return;}
	iRICUndoStack::instance().beginMacro(QObject::tr("Object Browser Item Change"));
	GraphicsWindowDataItem::handleStandardItemChange();
	if (m_standardItem->checkState() == Qt::Checked){
		// uncheck the node group dataitem.
		PreProcessorGridRelatedConditionNodeGroupDataItem* nitem = dynamic_cast<PreProcessorGridDataItem*>(parent())->nodeGroupDataItem();
		nitem->standardItem()->setCheckState(Qt::Unchecked);
	}
	iRICUndoStack::instance().endMacro();
}

void PreProcessorGridRelatedConditionCellGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid || gt == SolverDefinitionGridType::gtUnstructured2DGrid){
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridRelatedConditionCellGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_showAttributeBrowserAction);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::initAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid){
		pb->view()->resetForCell(true);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid){
		pb->view()->resetForCell(false);
	}
}

void PreProcessorGridRelatedConditionCellGroupDataItem::clearAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void PreProcessorGridRelatedConditionCellGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()){return;}

	vtkIdType cellid = findCell(p, v);
	m_attributeBrowserFixed = (cellid >= 0);
	if (cellid < 0){
		// it is not inside a cell.
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()){return;}
	if (m_attributeBrowserFixed){return;}

	vtkIdType cellid = findCell(p, v);
	if (cellid < 0){
		// it is not inside a cell.
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void PreProcessorGridRelatedConditionCellGroupDataItem::updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* /*v*/)
{
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*> (parent());
	Grid* grid = gitem->grid();

	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();

	QList<PropertyBrowserAttribute> atts;
	const QList<GridRelatedConditionContainer*> conds = grid->gridRelatedConditions();
	for (auto it = conds.begin(); it != conds.end(); ++it){
		GridRelatedConditionContainer* cond = *it;
		GridRelatedConditionIntegerCellContainer* icond = dynamic_cast<GridRelatedConditionIntegerCellContainer*>(cond);
		GridRelatedConditionRealCellContainer* rcond = dynamic_cast<GridRelatedConditionRealCellContainer*>(cond);
		if (icond != 0){
			if (icond->condition()->isOption()){
				SolverDefinitionIntegerOptionCellGridRelatedCondition* optCond =
						dynamic_cast<SolverDefinitionIntegerOptionCellGridRelatedCondition*>(icond->condition());
				PropertyBrowserAttribute att(icond->condition()->caption(), optCond->enumerations().value(icond->value(cellid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(icond->condition()->caption(), icond->value(cellid));
				atts.append(att);
			}
		} else if (rcond != 0){
			PropertyBrowserAttribute att(rcond->condition()->caption(), rcond->value(cellid));
			atts.append(att);
		}
	}
	QPolygonF polygon;
	vtkPoints* points = grid->vtkGrid()->GetPoints();
	vtkCell* cell = grid->vtkGrid()->GetCell(cellid);
	double *v;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i){
		v = points->GetPoint(cell->GetPointIds()->GetId(i));
		polygon.append(QPointF(*v, *(v + 1)));
	}
	v = points->GetPoint(cell->GetPointIds()->GetId(0));
	polygon.append(QPointF(*v, *(v + 1)));

	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*> (preProcessorWindow());
	if (gt == SolverDefinitionGridType::gtStructured2DGrid){
		Structured2DGrid* g2 = dynamic_cast<Structured2DGrid*>(gitem->grid());
		unsigned int i, j;
		g2->getCellIJIndex(cellid, &i, &j);
		pre->propertyBrowser()->view()->setCellAttributes(i, j, polygon, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid){
		pre->propertyBrowser()->view()->setCellAttributes(cellid, polygon, atts);
	}
}

vtkIdType PreProcessorGridRelatedConditionCellGroupDataItem::findCell(const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	PreProcessorGraphicsView* v2 = dynamic_cast<PreProcessorGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*> (parent());
	Grid* grid = gitem->grid();

	double point[3];
	point[0] = x; point[1] = y; point[2] = 0;
	vtkCell* hintCell = 0;
	double pcoords[4];
	double weights[4];
	int subid;
	vtkIdType cellid = grid->vtkGrid()->FindCell(point, hintCell, 0, 1e-4, subid, pcoords, weights);
	return cellid;
}

bool PreProcessorGridRelatedConditionCellGroupDataItem::addToolBarButtons(QToolBar *toolbar)
{
	PreProcessorGridRelatedConditionCellDataItem* activeItem = activeChildItem();
	if (activeItem == 0){return false;}
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*> (parent());
	Grid* grid = gitem->grid();
	GridRelatedConditionContainer* cont = grid->gridRelatedCondition(activeItem->condition()->name());
	const QList<GridRelatedConditionDimensionSelectWidget*>& selectWidgets = cont->dimensions()->selectWidgets();
	if (selectWidgets.size() == 0){return false;}
	for (int i = 0; i < selectWidgets.size(); ++i){
		GridRelatedConditionDimensionSelectWidget* w = selectWidgets.at(i);
		QAction* action = toolbar->addWidget(w);
		action->setVisible(true);
	}
	return true;
}
