#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "attributebrowserhelper.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "private/preprocessorgridattributenodegroupdataitem_modifyopacityandupdateactorsettingscommand.h"

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
#include <guicore/pre/gridcond/container/gridattributeintegernodecontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroptionnode.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
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

#include <vtkActor.h>
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

PreProcessorGridAttributeNodeGroupDataItem::PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* p) :
	PreProcessorDataItem {tr("Node attributes"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	p->standardItem()->takeRow(m_standardItem->row());
	PreProcessorGridTypeDataItem* typeItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	const QList<SolverDefinitionGridAttribute*>& conds = typeItem->gridType()->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() == SolverDefinitionGridAttribute::Node) {
			// this is a node condition.
			PreProcessorGridAttributeNodeDataItem* item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}

	const QList<SolverDefinitionGridComplexAttribute*>& compconds = typeItem->gridType()->gridComplexAttributes();
	for (auto cit = compconds.begin(); cit != compconds.end(); ++cit) {
		SolverDefinitionGridComplexAttribute* cond = *cit;
		if (cond->position() == SolverDefinitionGridComplexAttribute::Node) {
			// this is a node condition.
			PreProcessorGridAttributeNodeDataItem* item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert(item->condition()->name(), item);
		}
	}

	m_opacity = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(PreProcessorGridAttributeNodeGroupDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));

	m_actor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_actor);
}

PreProcessorGridAttributeNodeGroupDataItem::~PreProcessorGridAttributeNodeGroupDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
}

void PreProcessorGridAttributeNodeGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Node Attribute Change"));
	pushRenderCommand(cmd, this, true);
}

std::string PreProcessorGridAttributeNodeGroupDataItem::target() const
{
	return m_target;
}

void PreProcessorGridAttributeNodeGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void PreProcessorGridAttributeNodeGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();

	// make all the items invisible
	m_actorCollection->RemoveAllItems();
	Grid* g = dynamic_cast<PreProcessorGridDataItem*>(parent())->grid();
	if (g == 0) {
		// grid is not setup yet.
		return;
	}
	if (m_target == "") {
		return;
	}
	// update current active scalar
	vtkPointData* data = g->vtkGrid()->GetPointData();
	data->SetActiveScalars(m_target.c_str());
	auto typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	auto cs = typedi->colorMapSetting(m_target);

	auto algo = g->vtkFilteredCellsAlgorithm();
	algo->Update();
	auto mapper = cs->buildPointDataMapper(algo->GetOutput());
	m_actor->SetMapper(mapper);
	mapper->Delete();

	cs->legendSetting()->imgSetting()->apply(dataModel()->graphicsView());

	m_actor->GetProperty()->SetOpacity(m_opacity);

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void PreProcessorGridAttributeNodeGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacity.load(node);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		std::string name = dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it)->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "NodeAttribute", "name", name.c_str());
		if (! childNode.isNull()) {(*it)->loadFromProjectMainFile(childNode);}
	}
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeNodeDataItem* tmpItem = dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it);
		if (tmpItem->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setTarget(tmpItem->condition()->name());
		}
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_opacity.save(writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("NodeAttribute");
		writer.writeAttribute("name", dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it)->condition()->name().c_str());
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::informDataChange(const std::string& name)
{
	dynamic_cast<PreProcessorGridDataItem*>(parent())->informGridAttributeChange(name);
}

void PreProcessorGridAttributeNodeGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorGridAttributeNodeGroupDataItem::informSelection(VTKGraphicsView*)
{
	initAttributeBrowser();
}

void PreProcessorGridAttributeNodeGroupDataItem::informDeselection(VTKGraphicsView*)
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
	m_actor->SetPosition(0, 0, range.max());
}

void PreProcessorGridAttributeNodeGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridAttributeNodeGroupDataItem::activeChildItem()
{
	return m_nameMap.value(m_target);
}

const QList<PreProcessorGridAttributeNodeDataItem*> PreProcessorGridAttributeNodeGroupDataItem::conditions() const
{
	QList<PreProcessorGridAttributeNodeDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		ret.append(dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it));
	}
	return ret;
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridAttributeNodeGroupDataItem::nodeDataItem(const std::string& name) const
{
	return m_nameMap.value(name, nullptr);
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

void PreProcessorGridAttributeNodeGroupDataItem::setOpacityPercentAndUpdateActorSettings(int o, QUndoCommand* subcommand, bool renderOnRedoOnly)
{
	auto command = new ModifyOpacityAndUpdateActorSettingsCommand(o, subcommand, this);
	if (renderOnRedoOnly) {
		pushRenderRedoOnlyCommand(command, this);
	} else {
		pushRenderCommand(command, this);
	}
}

int PreProcessorGridAttributeNodeGroupDataItem::opacityPercent() const
{
	return m_opacity;
}

void PreProcessorGridAttributeNodeGroupDataItem::informSelectedVerticesChanged(const QVector<vtkIdType>& vertices)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGridAttributeNodeDataItem* item =
			dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(*it);
		item->informSelectedVerticesChanged(vertices);
	}
}

QAction* PreProcessorGridAttributeNodeGroupDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void PreProcessorGridAttributeNodeGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)) {
		PreProcessorWindow* pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	PreProcessorGridTypeDataItem* gtitem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent()->parent());
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gtitem)) {
		menu->addAction(m_showAttributeBrowserAction);
	}
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
	auto geoTopItem = gtitem->geoDataTop();
	SolverDefinitionGridType::GridType gt = gtitem->gridType()->defaultGridType();

	PreProcessorGridDataItem* gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	Grid* grid = gitem->grid();

	QList<PropertyBrowserAttribute> atts;
	const QList<GridAttributeContainer*> conds = grid->gridAttributes();
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		GridAttributeContainer* cond = *it;
		auto icond = dynamic_cast<GridAttributeIntegerNodeContainer*>(cond);
		auto rcond = dynamic_cast<GridAttributeRealNodeContainer*>(cond);
		auto ccond = dynamic_cast<GridComplexAttributeContainer*>(cond);

		if (icond != nullptr) {
			if (icond->gridAttribute()->isOption()) {
				SolverDefinitionGridAttributeIntegerOptionNode* optCond =
					dynamic_cast<SolverDefinitionGridAttributeIntegerOptionNode*>(icond->gridAttribute());
				PropertyBrowserAttribute att(icond->gridAttribute()->caption(), optCond->enumerations().value(icond->value(vid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(icond->gridAttribute()->caption(), icond->value(vid));
				atts.append(att);
			}
		} else if (rcond != nullptr) {
			PropertyBrowserAttribute att(rcond->gridAttribute()->caption(), rcond->value(vid));
			atts.append(att);
		} else if (ccond != nullptr) {
			auto group = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*>
					(geoTopItem->groupDataItem(ccond->name()));
			if (group->condition()->position() != SolverDefinitionGridComplexAttribute::Position::Node) {continue;}

			auto v = ccond->value(vid);
			GridComplexConditionGroup* g = group->groups().at(v - 1);
			PropertyBrowserAttribute att(group->condition()->caption(), g->caption());
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
	if (grid == nullptr) {
		// no grid
		return -1;
	}
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
	double limitDist = v2->stdRadius(iRIC::nearRadius());
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

void PreProcessorGridAttributeNodeGroupDataItem::applyColorMapSetting(const std::string& name)
{
	if (m_target != name) {return;}

	updateActorSettings();
}
