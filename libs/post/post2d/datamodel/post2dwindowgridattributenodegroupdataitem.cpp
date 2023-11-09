#include "../post2dwindow.h"
#include "../post2dwindowgraphicsview.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridattributenodedataitem.h"
#include "post2dwindowgridattributenodegroupdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridattributenodegroupdataitem_impl.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroption.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/qwidgetcontainer.h>
#include <misc/xmlsupport.h>

Post2dWindowGridAttributeNodeGroupDataItem::Impl::Impl(Post2dWindowGridAttributeNodeGroupDataItem* item) :
	m_target {},
	m_actor {vtkActor::New()},
	m_showAttributeBrowserAction {new QAction(Post2dWindowGridAttributeNodeGroupDataItem::tr("Show Attribute Browser"), item)},
	m_opacity {},
	m_attributeBrowserFixed {false},
	m_nameMap {},
	m_opacityWidget {new OpacityContainerWidget(item->mainWindow())},
	m_colorMapWidgetContainer {new QWidgetContainer(item->mainWindow())}
{
	m_actor->GetProperty()->SetLighting(false);
	m_opacity = 50;
}

Post2dWindowGridAttributeNodeGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
}

Post2dWindowGridAttributeNodeGroupDataItem::Post2dWindowGridAttributeNodeGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Node attributes"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	auto typeItem = gridTypeDataItem();
	const auto& conds = typeItem->gridType()->gridAttributes();
	for (auto cond : conds) {
		if (cond->position() == SolverDefinitionGridAttribute::Position::Node) {
			// this is a node condition.
			auto item = new Post2dWindowGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			impl->m_nameMap.insert({item->condition()->name(), item});
		}
	}

	const auto& compconds = typeItem->gridType()->gridComplexAttributes();
	for (auto cond : compconds) {
		if (cond->position() == SolverDefinitionGridComplexAttribute::Position::Node) {
			// this is a node condition.
			Post2dWindowGridAttributeNodeDataItem* item = new Post2dWindowGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			impl->m_nameMap.insert({item->condition()->name(), item});
		}
	}

	connect(impl->m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));

	renderer()->AddActor(impl->m_actor);

	impl->m_opacityWidget->setContainer(&impl->m_opacity);
	impl->m_opacityWidget->hide();

	connect(impl->m_opacityWidget, &OpacityContainerWidget::updated, [=]() {
		pushUpdateActorSettingCommand(impl->m_opacityWidget->createModifyCommand(), this);
	});

	impl->m_colorMapWidgetContainer->hide();
}

Post2dWindowGridAttributeNodeGroupDataItem::~Post2dWindowGridAttributeNodeGroupDataItem()
{
	renderer()->RemoveActor(impl->m_actor);
}

void Post2dWindowGridAttributeNodeGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Node Attribute Change"));
	pushRenderCommand(cmd, this, true);
}

std::string Post2dWindowGridAttributeNodeGroupDataItem::target() const
{
	return impl->m_target;
}

void Post2dWindowGridAttributeNodeGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_target = target;
	updateActorSetting();
}

void Post2dWindowGridAttributeNodeGroupDataItem::updateActorSetting()
{
	impl->m_opacityWidget->setDisabled(true);
	impl->m_actor->VisibilityOff();

	// make all the items invisible
	m_actorCollection->RemoveAllItems();
	v4InputGrid* g = gridDataItem()->inputGrid();
	if (g == nullptr) {
		// grid is not setup yet.
		return;
	}
	if (impl->m_target == "") {
		updateVisibilityWithoutRendering();
		return;
	}
	impl->m_opacityWidget->setEnabled(true);

	auto cs = gridTypeDataItem()->colorMapSetting(impl->m_target);

	auto grid2d = dynamic_cast<v4Grid2d*> (g->grid());
	auto filteredGrid = grid2d->vtkFilteredData();
	vtkPointData* data = filteredGrid->GetPointData();
	data->SetActiveScalars(impl->m_target.c_str());

	auto mapper = cs->customSetting->buildPointDataMapper(filteredGrid);
	impl->m_actor->SetMapper(mapper);
	mapper->Delete();

	impl->m_actor->GetProperty()->SetOpacity(impl->m_opacity);

	m_actorCollection->AddItem(impl->m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowGridAttributeNodeGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_opacity.load(node);
	for (auto item : conditions()) {
		const auto& name = item->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "NodeAttribute", "name", name.c_str());
		if (! childNode.isNull()) {item->loadFromProjectMainFile(childNode);}
	}

	for (auto child: m_childItems) {
		auto item = dynamic_cast<Post2dWindowGridAttributeNodeDataItem*> (child);
		if (item->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setTarget(item->condition()->name());
		}
	}
}

void Post2dWindowGridAttributeNodeGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_opacity.save(writer);
	for (auto item : conditions()) {
		writer.writeStartElement("NodeAttribute");
		writer.writeAttribute("name", item->condition()->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowGridAttributeNodeGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowGridAttributeNodeGroupDataItem::informSelection(VTKGraphicsView*)
{
	initAttributeBrowser();
}

void Post2dWindowGridAttributeNodeGroupDataItem::informDeselection(VTKGraphicsView*)
{
	clearAttributeBrowser();
}

QDialog* Post2dWindowGridAttributeNodeGroupDataItem::propertyDialog(QWidget* parent)
{
	auto child = activeChildItem();
	if (child == nullptr) {return nullptr;}

	return child->propertyDialog(parent);
}

void Post2dWindowGridAttributeNodeGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(event->pos(), v);
}

void Post2dWindowGridAttributeNodeGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(event->pos(), v);
}

void Post2dWindowGridAttributeNodeGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void Post2dWindowGridAttributeNodeGroupDataItem::update()
{
	updateActorSetting();
}

Post2dWindowGridAttributeNodeDataItem* Post2dWindowGridAttributeNodeGroupDataItem::activeChildItem() const
{
	return nodeDataItem(impl->m_target);
}

std::vector<Post2dWindowGridAttributeNodeDataItem*> Post2dWindowGridAttributeNodeGroupDataItem::conditions() const
{
	std::vector<Post2dWindowGridAttributeNodeDataItem*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<Post2dWindowGridAttributeNodeDataItem*>(child));
	}
	return ret;
}

Post2dWindowGridAttributeNodeDataItem* Post2dWindowGridAttributeNodeGroupDataItem::nodeDataItem(const std::string& name) const
{
	auto it = impl->m_nameMap.find(name);
	if (it == impl->m_nameMap.end()) {return nullptr;}

	return it->second;
}

void Post2dWindowGridAttributeNodeGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) {return;}

	iRICUndoStack::instance().beginMacro(QObject::tr("Object Browser Item Change"));
	GraphicsWindowDataItem::handleStandardItemChange();
	if (m_standardItem->checkState() == Qt::Checked) {
		// uncheck other group dataitems
		auto gItem = gridDataItem();
		for (auto child : gItem->childItems()) {
			if (child == this) {continue;}
			if (child == gItem->gridShapeDataItem()) {continue;}

			child->standardItem()->setCheckState(Qt::Unchecked);
		}
	}
	iRICUndoStack::instance().endMacro();
}

OpacityContainer& Post2dWindowGridAttributeNodeGroupDataItem::opacity()
{
	return impl->m_opacity;
}

OpacityContainerWidget* Post2dWindowGridAttributeNodeGroupDataItem::opacityWidget() const
{
	return impl->m_opacityWidget;
}

QWidgetContainer* Post2dWindowGridAttributeNodeGroupDataItem::colorMapWidgetContainer() const
{
	return impl->m_colorMapWidgetContainer;
}

QAction* Post2dWindowGridAttributeNodeGroupDataItem::showAttributeBrowserAction() const
{
	return impl->m_showAttributeBrowserAction;
}

void Post2dWindowGridAttributeNodeGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	post2dWindow()->propertyBrowser()->show();
}

void Post2dWindowGridAttributeNodeGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_showAttributeBrowserAction);
}

void Post2dWindowGridAttributeNodeGroupDataItem::initAttributeBrowser()
{
	auto pb = post2dWindow()->propertyBrowser();
	if (dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()) != nullptr) {
		pb->view()->resetForVertex(true);
	} else {
		pb->view()->resetForVertex(false);
	}
}

void Post2dWindowGridAttributeNodeGroupDataItem::clearAttributeBrowser()
{
	auto pb = post2dWindow()->propertyBrowser();
	pb->view()->hideAll();
	impl->m_attributeBrowserFixed = false;
}

void Post2dWindowGridAttributeNodeGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pb = post2dWindow()->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType vid = findVertex(p, v);
	impl->m_attributeBrowserFixed = (vid >= 0);
	if (vid < 0) {
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	auto grid = gridDataItem()->inputGrid();
	grid->grid()->vtkData()->data()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void Post2dWindowGridAttributeNodeGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pb = post2dWindow()->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (impl->m_attributeBrowserFixed) {return;}

	vtkIdType vid = findVertex(p, v);
	if (vid < 0) {
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	auto grid = gridDataItem()->grid();
	grid->vtkData()->data()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void Post2dWindowGridAttributeNodeGroupDataItem::updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* /*v*/)
{
	auto gtItem = gridTypeDataItem();
	auto geoTopItem = gtItem->geoDataItem();
	SolverDefinitionGridType::GridType gt = gtItem->gridType()->defaultGridType();

	v4InputGrid* grid = gridDataItem()->inputGrid();

	QList<PropertyBrowserAttribute> atts;
	for (auto att : grid->attributes()) {
		if (att->gridAttribute()->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}

		auto iAtt = dynamic_cast<GridAttributeIntegerContainer*>(att);
		auto rAtt = dynamic_cast<GridAttributeRealContainer*>(att);
		auto cAtt = dynamic_cast<GridComplexAttributeContainer*>(att);

		if (iAtt != nullptr) {
			if (iAtt->gridAttribute()->isOption()) {
				auto optCond = dynamic_cast<SolverDefinitionGridAttributeIntegerOption*>(iAtt->gridAttribute());
				PropertyBrowserAttribute att(iAtt->gridAttribute()->caption(), optCond->enumerations().value(iAtt->value(vid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(iAtt->gridAttribute()->caption(), iAtt->value(vid));
				atts.append(att);
			}
		} else if (rAtt!= nullptr) {
			PropertyBrowserAttribute att(rAtt->gridAttribute()->caption(), rAtt->value(vid));
			atts.append(att);
		} else if (cAtt!= nullptr) {
			auto group1 = geoTopItem->preGeoDataTopDataItem()->groupDataItem(cAtt->name());
			auto group2 = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (group1);
			if (group2->condition()->position() != SolverDefinitionGridComplexAttribute::Position::Node) {continue;}

			auto v = cAtt->value(vid);
			auto g = group2->groups().at(v - 1);
			PropertyBrowserAttribute att(group2->condition()->caption(), g->caption());
			atts.append(att);
		}
	}
	auto pb = post2dWindow()->propertyBrowser();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		auto g2 = dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid());
		vtkIdType i, j;
		g2->getPointIJIndex(vid, &i, &j);
		pb->view()->setVertexAttributes(i, j, x, y, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->setVertexAttributes(vid, x, y, atts);
	}
}

Post2dWindowGridTypeDataItem* Post2dWindowGridAttributeNodeGroupDataItem::gridTypeDataItem() const
{
	return gridDataItem()->zoneDataItem()->gridTypeDataItem();
}

Post2dWindow* Post2dWindowGridAttributeNodeGroupDataItem::post2dWindow() const
{
	return dynamic_cast<Post2dWindow*> (mainWindow());
}

vtkIdType Post2dWindowGridAttributeNodeGroupDataItem::findVertex(const QPoint& p, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	QPointF pos = v2->VTK2DGraphicsView::viewportToWorld(p);

	v4InputGrid* grid = gridDataItem()->inputGrid();
	if (grid == nullptr) {
		// no grid
		return -1;
	}
	double limitDist = v2->stdRadius(iRIC::nearRadius());
	double point[3] = {pos.x(), pos.y(), 0};
	double dist;
	vtkIdType vid = grid->grid()->vtkData()->pointLocator()->FindClosestPointWithinRadius(limitDist, point, dist);
	if (vid < 0) {
		// no point is near.
		post2dWindow()->propertyBrowser()->view()->resetAttributes();
		return -1;
	}

	return vid;
}

bool Post2dWindowGridAttributeNodeGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_opacityWidget->setParent(toolBar);
	impl->m_opacityWidget->show();
	toolBar->addWidget(impl->m_opacityWidget);

	toolBar->addSeparator();

	impl->m_colorMapWidgetContainer->setParent(toolBar);
	impl->m_colorMapWidgetContainer->show();
	toolBar->addWidget(impl->m_colorMapWidgetContainer);

	auto activeItem = activeChildItem();
	if (activeItem == 0) {return true;}

	auto grid = gridDataItem()->inputGrid();
	if (grid == nullptr) {return true;}

	auto att = grid->attribute(activeItem->condition()->name());
	const auto& selectWidgets = att->dimensions()->selectWidgets();
	if (selectWidgets.size() > 0) {
		toolBar->addSeparator();

		for (int i = 0; i < static_cast<int> (selectWidgets.size()); ++i) {
			auto w = selectWidgets.at(i);
			QAction* action = toolBar->addWidget(w);
			action->setVisible(true);
		}
	}

	return true;
}

void Post2dWindowGridAttributeNodeGroupDataItem::applyColorMapSetting(const std::string& name)
{
	if (impl->m_target != name) {return;}

	updateActorSetting();
}

Post2dWindowInputGridDataItem* Post2dWindowGridAttributeNodeGroupDataItem::gridDataItem() const
{
	return dynamic_cast<Post2dWindowInputGridDataItem*> (parent());
}

bool Post2dWindowGridAttributeNodeGroupDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	auto active = activeChildItem();
	if (active == nullptr) {return false;}

	if (active->name() != name) {return false;}
	if (! active->isAncientChecked()) {return false;}
	if (! active->isChecked()) {return false;}

	return true;
}
