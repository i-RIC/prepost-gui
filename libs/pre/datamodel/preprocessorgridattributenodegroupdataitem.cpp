#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegernodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroptionnode.h>
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
#include <vtkCellData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>

PreProcessorGridAttributeNodeGroupDataItem::PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* p)
	: PreProcessorDataItem(tr("Node attributes"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Unchecked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
	p->standardItem()->takeRow(m_standardItem->row());
	PreProcessorGridTypeDataItem* typeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	const QList<SolverDefinitionGridAttribute*>& conds = typeItem->gridType()->gridRelatedConditions();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() == SolverDefinitionGridAttribute::Node) {
			// this is a node condition.
			PreProcessorGridAttributeNodeDataItem* item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.append(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}

	const QList<SolverDefinitionGridComplexAttribute*>& compconds = typeItem->gridType()->gridRelatedComplexConditions();
	for (auto cit = compconds.begin(); cit != compconds.end(); ++cit) {
		SolverDefinitionGridComplexAttribute* cond = *cit;
		if (cond->position() == SolverDefinitionGridComplexAttribute::Node) {
			// this is a node condition.
			PreProcessorGridAttributeNodeDataItem* item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.append(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}

	m_opacityPercent = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
}
PreProcessorGridAttributeNodeGroupDataItem::~PreProcessorGridAttributeNodeGroupDataItem()
{
	renderer()->RemoveActor(m_isolineActor);
	renderer()->RemoveActor(m_contourActor);
	renderer()->RemoveActor(m_fringeActor);
}

class PreProcessorSelectCondition : public QUndoCommand
{
public:
	PreProcessorSelectCondition(const QString& newcond, PreProcessorGridAttributeNodeGroupDataItem* item)
		: QUndoCommand(QObject::tr("Node Attribute Change")) {
		m_newCurrentCondition = newcond;
		m_oldCurrentCondition = item->m_currentCondition;
		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentCondition(m_oldCurrentCondition);
		m_item->updateActorSettings();
		PreProcessorGridDataItem* gItem =
			dynamic_cast<PreProcessorGridDataItem*>(m_item->parent());
		gItem->updateSimplifiedGrid();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		m_item->setCurrentCondition(m_newCurrentCondition);
		m_item->updateActorSettings();
		PreProcessorGridDataItem* gItem =
			dynamic_cast<PreProcessorGridDataItem*>(m_item->parent());
		gItem->updateSimplifiedGrid();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	QString m_oldCurrentCondition;
	QString m_newCurrentCondition;

	PreProcessorGridAttributeNodeGroupDataItem* m_item;
};

void PreProcessorGridAttributeNodeGroupDataItem::exclusivelyCheck(PreProcessorGridAttributeNodeDataItem* item)
{
	if (m_isCommandExecuting) {return;}
	iRICUndoStack& stack = iRICUndoStack::instance();
	if (item->standardItem()->checkState() != Qt::Checked) {
		stack.push(new PreProcessorSelectCondition("", this));
	} else {
		stack.push(new PreProcessorSelectCondition(item->condition()->name(), this));
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::setCurrentCondition(const QString& currentCond)
{
	PreProcessorGridAttributeNodeDataItem* current = 0;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeNodeDataItem* tmpItem = dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it);
		if (tmpItem->condition()->name() == currentCond) {
			current = tmpItem;
		}
		tmpItem->standardItem()->setCheckState(Qt::Unchecked);
	}
	if (current != 0) {
		current->standardItem()->setCheckState(Qt::Checked);
	}
	m_currentCondition = currentCond;
}

void PreProcessorGridAttributeNodeGroupDataItem::updateActorSettings()
{
	// make all the items invisible
	m_isolineActor->VisibilityOff();
	m_contourActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
	m_actorCollection->RemoveAllItems();
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == 0) {
		// grid is not setup yet.
		return;
	}
	if (m_currentCondition == "") {
		updateVisibilityWithoutRendering();
		return;
	}
	// update current active scalar
	vtkPointData* data = g->vtkGrid()->GetPointData();
	data->SetActiveScalars(iRIC::toStr(m_currentCondition).c_str());
	PreProcessorGridAttributeNodeDataItem* activeItem = activeChildItem();
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(iRIC::toStr(m_currentCondition).c_str());

	double range[2];
	stc->getValueRange(&range[0], &range[1]);

	switch (activeItem->contour()) {
	case ContourSettingWidget::Isolines:
		m_isolineFilter->GenerateValues(activeItem->numberOfDivision() + 1, range);
		m_isolineMapper->SetLookupTable(stc->vtkObj());
		m_isolineMapper->UseLookupTableScalarRangeOn();
		m_actorCollection->AddItem(m_isolineActor);
		break;
	case ContourSettingWidget::ContourFigure: {
			vtkSmartPointer<vtkAppendPolyData> appendFilledContours = vtkSmartPointer<vtkAppendPolyData>::New();
			double delta = (range[1] - range[0]) / static_cast<double>(activeItem->numberOfDivision());
			std::vector< vtkSmartPointer<vtkClipPolyData> > clippersLo;
			std::vector< vtkSmartPointer<vtkClipPolyData> > clippersHi;

			for (int i = 0; i < activeItem->numberOfDivision(); i++) {
				double valueLo = range[0] + static_cast<double>(i) * delta;
				double valueHi = range[0] + static_cast<double>(i + 1) * delta;
				clippersLo.push_back(vtkSmartPointer<vtkClipPolyData>::New());
				if (i == 0) {
					clippersLo[i]->SetValue(- HUGE_VAL);
					// make PolyData from PointSet
					vtkSmartPointer<vtkStructuredGridGeometryFilter> filter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
					filter->SetInputData(g->vtkGrid());
					filter->Update();
					clippersLo[i]->SetInputConnection(filter->GetOutputPort());
				} else {
					clippersLo[i]->SetValue(valueLo);
					clippersLo[i]->SetInputConnection(clippersLo[i - 1]->GetOutputPort());
				}
				clippersLo[i]->InsideOutOff();
				clippersLo[i]->Update();

				clippersHi.push_back(vtkSmartPointer<vtkClipPolyData>::New());
				clippersHi[i]->SetValue(valueHi);
				clippersHi[i]->SetInputConnection(clippersLo[i]->GetOutputPort());
				clippersHi[i]->GenerateClippedOutputOn();
				clippersHi[i]->InsideOutOn();
				clippersHi[i]->Update();
				if (clippersHi[i]->GetOutput()->GetNumberOfCells() == 0) {
					continue;
				}

				vtkSmartPointer<vtkDoubleArray> cd = vtkSmartPointer<vtkDoubleArray>::New();
				cd->SetNumberOfComponents(1);
				cd->SetNumberOfTuples(clippersHi[i]->GetOutput()->GetNumberOfCells());
				cd->FillComponent(0, range[0] + (range[1] - range[0]) * (i / (activeItem->numberOfDivision() - 1.0)));

				clippersHi[i]->GetOutput()->GetCellData()->SetScalars(cd);
				appendFilledContours->AddInputConnection(clippersHi[i]->GetOutputPort());
			}

			vtkSmartPointer<vtkCleanPolyData> filledContours = vtkSmartPointer<vtkCleanPolyData>::New();
			filledContours->SetInputConnection(appendFilledContours->GetOutputPort());
			filledContours->Update();
			m_contourPolyData->DeepCopy(filledContours->GetOutput());
			m_contourMapper->SetInputData(m_contourPolyData);
			m_contourMapper->SetScalarRange(range[0], range[1]);
			m_contourMapper->SetScalarModeToUseCellData();
			m_contourActor->GetProperty()->SetInterpolationToFlat();
			m_contourActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
			m_contourMapper->SetLookupTable(stc->vtkObj());
			m_contourMapper->UseLookupTableScalarRangeOn();
			if (m_nameMap.value(m_currentCondition)->condition()->isOption()) {} else {
				m_contourMapper->SetLookupTable(stc->vtkObj());
			}
			m_actorCollection->AddItem(m_contourActor);
		}
		break;
	case ContourSettingWidget::ColorFringe:
		m_fringeMapper->SetScalarModeToUsePointData();
		m_fringeMapper->SetLookupTable(stc->vtkObj());
		m_fringeMapper->UseLookupTableScalarRangeOn();
		m_fringeActor->GetProperty()->SetOpacity(m_opacityPercent / 100.);
		m_actorCollection->AddItem(m_fringeActor);
		break;
	default:
		break;
	}
	updateVisibilityWithoutRendering();
}

void PreProcessorGridAttributeNodeGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacityPercent = loadOpacityPercent(node);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		QString name = dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it)->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "NodeAttribute", "name", name);
		if (! childNode.isNull()) {(*it)->loadFromProjectMainFile(childNode);}
	}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeNodeDataItem* tmpItem = dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it);
		if (tmpItem->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setCurrentCondition(tmpItem->condition()->name());
		}
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writeOpacityPercent(m_opacityPercent, writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("NodeAttribute");
		writer.writeAttribute("name", dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it)->condition()->name());
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::informDataChange(const QString& name)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->informgridRelatedConditionChange(name);
}


void PreProcessorGridAttributeNodeGroupDataItem::setupActors()
{
	m_contourActor = vtkSmartPointer<vtkActor>::New();
	// Make the point size a little big.
	m_contourActor->GetProperty()->SetPointSize(2);
	m_contourActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_contourActor);

	m_fringeActor = vtkSmartPointer<vtkActor>::New();
	m_fringeActor->GetProperty()->SetPointSize(2);
	m_fringeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_fringeActor);

	m_contourPolyData = vtkSmartPointer<vtkPolyData>::New();

	m_contourMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_contourMapper->SetScalarVisibility(true);
	m_contourActor->SetMapper(m_contourMapper);

	m_fringeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_fringeMapper->SetScalarVisibility(true);
	m_fringeActor->SetMapper(m_fringeMapper);

	m_isolineActor = vtkSmartPointer<vtkActor>::New();
	m_isolineActor->GetProperty()->SetLighting(false);
	m_isolineActor->GetProperty()->SetLineWidth(1);
	renderer()->AddActor(m_isolineActor);

	m_isolineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_isolineActor->SetMapper(m_isolineMapper);
	m_isolineFilter = vtkSmartPointer<vtkContourFilter>::New();
	m_isolineMapper->SetInputConnection(m_isolineFilter->GetOutputPort());

	m_contourActor->VisibilityOff();
	m_isolineActor->VisibilityOff();
	m_fringeActor->VisibilityOff();
}

void PreProcessorGridAttributeNodeGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorGridAttributeNodeGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	initAttributeBrowser();
}

void PreProcessorGridAttributeNodeGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	clearAttributeBrowser();
}

void PreProcessorGridAttributeNodeGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridAttributeNodeGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void PreProcessorGridAttributeNodeGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_contourActor->SetPosition(0, 0, range.min());
	m_isolineActor->SetPosition(0, 0, range.max());
	m_fringeActor->SetPosition(0, 0, range.min());
}

void PreProcessorGridAttributeNodeGroupDataItem::informGridUpdate()
{
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g != 0) {
//		m_isolineFilter->SetInputData(g->vtkGrid());
//		m_fringeMapper->SetInputData(g->vtkGrid());
		vtkAlgorithm* cellsAlgo = g->vtkFilteredCellsAlgorithm();
		if (cellsAlgo != 0) {
			m_isolineFilter->SetInputConnection(cellsAlgo->GetOutputPort());
			m_fringeMapper->SetInputConnection(cellsAlgo->GetOutputPort());
		}
	}
	updateActorSettings();
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridAttributeNodeGroupDataItem::activeChildItem()
{
	return m_nameMap.value(m_currentCondition);
}

const QList<PreProcessorGridAttributeNodeDataItem*> PreProcessorGridAttributeNodeGroupDataItem::conditions() const
{
	QList<PreProcessorGridAttributeNodeDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		ret.append(dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it));
	}
	return ret;
}

void PreProcessorGridAttributeNodeGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) {return;}
	iRICUndoStack::instance().beginMacro(QObject::tr("Object Browser Item Change"));
	GraphicsWindowDataItem::handleStandardItemChange();
	if (m_standardItem->checkState() == Qt::Checked) {
		// uncheck the node group dataitem.
		PreProcessorGridAttributeCellGroupDataItem* nitem = dynamic_cast<PreProcessorGridDataItem*>(parent())->cellGroupDataItem();
		nitem->standardItem()->setCheckState(Qt::Unchecked);
	}
	iRICUndoStack::instance().endMacro();
}

void PreProcessorGridAttributeNodeGroupDataItem::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeNodeDataItem* item =
			dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it);
		item->informSelectedVerticesChanged(vertices);
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid || gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_showAttributeBrowserAction);
}

void PreProcessorGridAttributeNodeGroupDataItem::initAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		pb->view()->resetForVertex(true);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->resetForVertex(false);
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::clearAttributeBrowser()
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void PreProcessorGridAttributeNodeGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType vid = findVertex(p, v);
	m_attributeBrowserFixed = (vid >= 0);
	if (vid < 0) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}
	double vertex[3];
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();
	grid->vtkGrid()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void PreProcessorGridAttributeNodeGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType vid = findVertex(p, v);
	if (vid < 0) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->view()->resetAttributes();
		return;
	}
	double vertex[3];
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();
	grid->vtkGrid()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void PreProcessorGridAttributeNodeGroupDataItem::updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* /*v*/)
{
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();

	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();

	QList<PropertyBrowserAttribute> atts;
	const QList<GridAttributeContainer*> conds = grid->gridRelatedConditions();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		GridAttributeContainer* cond = *it;
		GridAttributeIntegerNodeContainer* icond = dynamic_cast<GridAttributeIntegerNodeContainer*>(cond);
		GridAttributeRealNodeContainer* rcond = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
		if (icond != 0) {
			if (icond->condition()->isOption()) {
				SolverDefinitionGridAttributeIntegerOptionNode* optCond =
					dynamic_cast<SolverDefinitionGridAttributeIntegerOptionNode*>(icond->condition());
				PropertyBrowserAttribute att(icond->condition()->caption(), optCond->enumerations().value(icond->value(vid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(icond->condition()->caption(), icond->value(vid));
				atts.append(att);
			}
		} else if (rcond != 0) {
			PropertyBrowserAttribute att(rcond->condition()->caption(), rcond->value(vid));
			atts.append(att);
		}
	}
	PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		Structured2DGrid* g2 = dynamic_cast<Structured2DGrid*>(gitem->grid());
		unsigned int i, j;
		g2->getIJIndex(vid, &i, &j);
		pre->propertyBrowser()->view()->setVertexAttributes(i, j, x, y, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pre->propertyBrowser()->view()->setVertexAttributes(vid, x, y, atts);
	}
}

vtkIdType PreProcessorGridAttributeNodeGroupDataItem::findVertex(const QPoint& p, VTKGraphicsView* v)
{
	double x = p.x();
	double y = p.y();
	PreProcessorGraphicsView* v2 = dynamic_cast<PreProcessorGraphicsView*>(v);
	v2->viewportToWorld(x, y);

	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();
	vtkIdType vid = grid->vtkGrid()->FindPoint(x, y, 0);
	if (vid < 0) {
		// no point is near.
		return -1;
	}
	double vertex[3];
	grid->vtkGrid()->GetPoint(vid, vertex);

	QVector2D vec1(x, y);
	QVector2D vec2(vertex[0], vertex[1]);
	double distance = (vec2 - vec1).length();
	double limitDist = v2->stdRadius(5);
	if (distance > limitDist) {
		// no point is near.
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->view()->resetAttributes();
		return -1;
	}
	return vid;
}

bool PreProcessorGridAttributeNodeGroupDataItem::addToolBarButtons(QToolBar* toolbar)
{
	PreProcessorGridAttributeNodeDataItem* activeItem = activeChildItem();
	if (activeItem == 0) {return false;}
	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();
	GridAttributeContainer* cont = grid->gridRelatedCondition(activeItem->condition()->name());
	const QList<GridAttributeDimensionSelectWidget*>& selectWidgets = cont->dimensions()->selectWidgets();
	if (selectWidgets.size() == 0) {return false;}
	for (int i = 0; i < selectWidgets.size(); ++i) {
		GridAttributeDimensionSelectWidget* w = selectWidgets.at(i);
		QAction* action = toolbar->addWidget(w);
		action->setVisible(true);
	}
	return true;
}
