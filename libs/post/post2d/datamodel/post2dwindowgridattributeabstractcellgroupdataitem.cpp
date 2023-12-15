#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgeodatatopdataitem.h"
#include "post2dwindowgridattributeabstractcelldataitem.h"
#include "post2dwindowgridattributeabstractcellgroupdataitem.h"
#include "post2dwindowgridshapedataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowinputgriddataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowgridattributeabstractcellgroupdataitem_impl.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroption.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/qwidgetcontainer.h>
#include <misc/xmlsupport.h>

Post2dWindowGridAttributeAbstractCellGroupDataItem::Impl::Impl(Post2dWindowGridAttributeAbstractCellGroupDataItem* item) :
	m_target {},
	m_actor {vtkActor::New()},
	m_nameMap {},
	m_lineWidth {"lineWidth", 5},
	m_opacity {},
	m_showAttributeBrowserAction {new QAction(Post2dWindowGridAttributeAbstractCellGroupDataItem::tr("Show Attribute Browser"), item)},
	m_attributeBrowserFixed {false},
	m_opacityWidget {new OpacityContainerWidget(item->mainWindow())},
	m_colorMapWidgetContainer {new QWidgetContainer(item->mainWindow())}
{
	m_opacity = 50;
}

Post2dWindowGridAttributeAbstractCellGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
}

Post2dWindowGridAttributeAbstractCellGroupDataItem::Post2dWindowGridAttributeAbstractCellGroupDataItem(const QString& caption, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(impl->m_showAttributeBrowserAction, &QAction::triggered, this, &Post2dWindowGridAttributeAbstractCellGroupDataItem::showAttributeBrowser);

	impl->m_actor->GetProperty()->SetLighting(false);
	impl->m_actor->GetProperty()->SetLineWidth(5);
	renderer()->AddActor(impl->m_actor);

	impl->m_opacityWidget->setContainer(&impl->m_opacity);
	impl->m_opacityWidget->hide();

	connect(impl->m_opacityWidget, &OpacityContainerWidget::updated, [=]() {
			pushUpdateActorSettingCommand(impl->m_opacityWidget->createModifyCommand(), this);
	});

	impl->m_colorMapWidgetContainer->hide();
}

Post2dWindowGridAttributeAbstractCellGroupDataItem::~Post2dWindowGridAttributeAbstractCellGroupDataItem()
{
	renderer()->RemoveActor(impl->m_actor);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::setupChildren()
{
	auto pos = definitionPosition();

	auto typeItem = gridTypeDataItem();
	const auto& conds = typeItem->gridType()->gridAttributes();
	for (auto cond : conds) {
		if (cond->position() == pos) {
			auto item = createChild(cond);
			m_childItems.push_back(item);
			impl->m_nameMap.insert({item->condition()->name(), item});
		}
	}
	const auto& compConds = typeItem->gridType()->gridComplexAttributes();
	for (auto cond : compConds) {
		if (cond->position() == pos) {
			auto item = createChild(cond);
			m_childItems.push_back(item);
			impl->m_nameMap.insert({item->condition()->name(), item});
		}
	}
}

std::string Post2dWindowGridAttributeAbstractCellGroupDataItem::target() const
{
	return impl->m_target;
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_target = target;
	updateActorSetting();
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Cell Attribute Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::updateActorSetting()
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

	auto filteredGrid = filteredData();
	vtkCellData* data = filteredGrid->GetCellData();
	data->SetActiveScalars(impl->m_target.c_str());

	auto mapper = cs->customSetting->buildCellDataMapper(filteredGrid, false);
	impl->m_actor->SetMapper(mapper);
	mapper->Delete();

	impl->m_actor->GetProperty()->SetOpacity(impl->m_opacity);
	impl->m_actor->GetProperty()->SetLineWidth(impl->m_lineWidth);

	m_actorCollection->AddItem(impl->m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	initAttributeBrowser();
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	clearAttributeBrowser();
}

QDialog* Post2dWindowGridAttributeAbstractCellGroupDataItem::propertyDialog(QWidget* parent)
{
	auto child = activeChildItem();
	if (child == nullptr) {return nullptr;}

	return child->propertyDialog(parent);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(event->pos(), v);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(event->pos(), v);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::update()
{
	updateActorSetting();
}

Post2dWindowGridAttributeAbstractCellDataItem* Post2dWindowGridAttributeAbstractCellGroupDataItem::activeChildItem() const
{
	return cellDataItem(impl->m_target);
}

std::vector<Post2dWindowGridAttributeAbstractCellDataItem*> Post2dWindowGridAttributeAbstractCellGroupDataItem::conditions() const
{
	std::vector<Post2dWindowGridAttributeAbstractCellDataItem*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<Post2dWindowGridAttributeAbstractCellDataItem*>(child));
	}
	return ret;
}

Post2dWindowGridAttributeAbstractCellDataItem* Post2dWindowGridAttributeAbstractCellGroupDataItem::cellDataItem(const std::string& name) const
{
	auto it = impl->m_nameMap.find(name);
	if (it == impl->m_nameMap.end()) {return nullptr;}

	return it->second;
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::handleStandardItemChange()
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

IntContainer& Post2dWindowGridAttributeAbstractCellGroupDataItem::lineWidth()
{
	return impl->m_lineWidth;
}

OpacityContainer& Post2dWindowGridAttributeAbstractCellGroupDataItem::opacity()
{
	return impl->m_opacity;
}

OpacityContainerWidget* Post2dWindowGridAttributeAbstractCellGroupDataItem::opacityWidget() const
{
	return impl->m_opacityWidget;
}

QWidgetContainer* Post2dWindowGridAttributeAbstractCellGroupDataItem::colorMapWidgetContainer() const
{
	return impl->m_colorMapWidgetContainer;
}

QAction* Post2dWindowGridAttributeAbstractCellGroupDataItem::showAttributeBrowserAction() const
{
	return impl->m_showAttributeBrowserAction;
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	post2dWindow()->propertyBrowser()->show();
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_showAttributeBrowserAction);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::initAttributeBrowser()
{
	auto pb = post2dWindow()->propertyBrowser();
	if (dynamic_cast<v4Structured2dGrid*> (gridDataItem()->grid()) != nullptr) {
		pb->view()->resetForVertex(true);
	} else {
		pb->view()->resetForVertex(false);
	}
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::clearAttributeBrowser()
{
	auto pb = post2dWindow()->propertyBrowser();
	pb->view()->hideAll();
	impl->m_attributeBrowserFixed = false;
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pb = post2dWindow()->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType cellid = findCell(p, v);
	impl->m_attributeBrowserFixed = (cellid >= 0);
	if (cellid < 0) {
		pb->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pb = post2dWindow()->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (impl->m_attributeBrowserFixed) {return;}

	vtkIdType cellid = findCell(p, v);
	if (cellid < 0) {
		// it is not inside a cell.
		pb->view()->resetAttributes();
		return;
	}

	updateAttributeBrowser(cellid, v);
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* /*v*/)
{
	auto gtItem = gridTypeDataItem();
	auto geoTopItem = gtItem->geoDataItem();
	SolverDefinitionGridType::GridType gt = gtItem->gridType()->defaultGridType();

	v4InputGrid* grid = gridDataItem()->inputGrid();

	QList<PropertyBrowserAttribute> atts;
	auto pos = definitionPosition();
	for (auto att : grid->attributes()) {
		if (att->gridAttribute()->position() != pos) {continue;}

		auto iAtt = dynamic_cast<GridAttributeIntegerContainer*>(att);
		auto rAtt = dynamic_cast<GridAttributeRealContainer*>(att);
		auto cAtt = dynamic_cast<GridComplexAttributeContainer*>(att);

		if (iAtt != nullptr) {
			if (iAtt->gridAttribute()->isOption()) {
				auto optCond = dynamic_cast<SolverDefinitionGridAttributeIntegerOption*>(iAtt->gridAttribute());
				PropertyBrowserAttribute att(iAtt->gridAttribute()->caption(), optCond->enumerations().value(iAtt->value(cellid)));
				atts.append(att);
			} else {
				PropertyBrowserAttribute att(iAtt->gridAttribute()->caption(), iAtt->value(cellid));
				atts.append(att);
			}
		} else if (rAtt != nullptr) {
			PropertyBrowserAttribute att(rAtt->gridAttribute()->caption(), rAtt->value(cellid));
			atts.append(att);
		} else if (cAtt != nullptr) {
			auto group1 = geoTopItem->preGeoDataTopDataItem()->groupDataItem(cAtt->name());
			auto group2 = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*> (group1);
			if (group2->condition()->position() != pos) {continue;}

			auto v = cAtt->value(cellid);
			if (v == 0) {v = 1;} // in case not mapped yet, change value to Default (= 1)
			auto g = group2->groups().at(v - 1);
			PropertyBrowserAttribute att(group2->condition()->caption(), g->caption());
			atts.append(att);
		}
	}
	QPolygonF polygon;
	vtkPointSet* vtkGrid = data()->data();
	vtkPoints* points = vtkGrid->GetPoints();
	vtkCell* cell = vtkGrid->GetCell(cellid);
	double* v;
	for (vtkIdType i = 0; i < cell->GetNumberOfPoints(); ++i) {
		v = points->GetPoint(cell->GetPointIds()->GetId(i));
		polygon.append(QPointF(*v, *(v + 1)));
	}
	v = points->GetPoint(cell->GetPointIds()->GetId(0));
	polygon.append(QPointF(*v, *(v + 1)));

	auto pb = post2dWindow()->propertyBrowser();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		vtkIdType i, j;
		getIJIndex(cellid, &i, &j);
		pb->view()->setCellAttributes(i, j, polygon, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->setCellAttributes(cellid, polygon, atts);
	}
}

Post2dWindowGridTypeDataItem* Post2dWindowGridAttributeAbstractCellGroupDataItem::gridTypeDataItem() const
{
	return gridDataItem()->zoneDataItem()->gridTypeDataItem();
}

Post2dWindow* Post2dWindowGridAttributeAbstractCellGroupDataItem::post2dWindow() const
{
	return dynamic_cast<Post2dWindow*> (mainWindow());
}

vtkIdType Post2dWindowGridAttributeAbstractCellGroupDataItem::findCell(const QPoint& p, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<Post2dWindowGraphicsView*>(v);
	QPointF pos = v2->VTK2DGraphicsView::viewportToWorld(p);

	auto gItem = gridDataItem();
	v4InputGrid* grid = gItem->inputGrid();
	if (grid == nullptr) {
		return -1;
	}

	double weights[4];
	return data()->findCell(pos.x(), pos.y(), 0, v2->stdRadius(iRIC::nearRadius()), weights);
}

bool Post2dWindowGridAttributeAbstractCellGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	impl->m_opacityWidget->setParent(toolBar);
	impl->m_opacityWidget->show();
	toolBar->addWidget(impl->m_opacityWidget);

	toolBar->addSeparator();

	impl->m_colorMapWidgetContainer->setParent(toolBar);
	impl->m_colorMapWidgetContainer->show();
	toolBar->addWidget(impl->m_colorMapWidgetContainer);

	auto activeItem = activeChildItem();
	if (activeItem == nullptr) {return true;}

	auto grid = gridDataItem()->inputGrid();
	if (grid == nullptr) {return true;}

	auto att = grid->attribute(activeItem->condition()->name());
	const auto& selectWidgets = att->dimensions()->selectWidgets();
	if (selectWidgets.size() >= 0) {
		toolBar->addSeparator();

		for (int i = 0; i < static_cast<int> (selectWidgets.size()); ++i) {
			GridAttributeDimensionSelectWidget* w = selectWidgets.at(i);
			QAction* action = toolBar->addWidget(w);
			action->setVisible(true);
		}
	}

	return true;
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::applyColorMapSetting(const std::string& name)
{
	if (impl->m_target != name) {return;}

	updateActorSetting();
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_opacity.load(node);
	for (auto item : conditions()) {
		const auto& name = item->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "CellAttribute", "name", name.c_str());
		if (! childNode.isNull()) {item->loadFromProjectMainFile(childNode);}
	}
	for (auto item : conditions()) {
		if (item->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setTarget(item->condition()->name());
		}
	}
}

void Post2dWindowGridAttributeAbstractCellGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_opacity.save(writer);
	for (auto item : conditions()) {
		writer.writeStartElement("CellAttribute");
		writer.writeAttribute("name", item->condition()->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

Post2dWindowInputGridDataItem* Post2dWindowGridAttributeAbstractCellGroupDataItem::gridDataItem() const
{
	return dynamic_cast<Post2dWindowInputGridDataItem*> (parent());
}

bool Post2dWindowGridAttributeAbstractCellGroupDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	auto active = activeChildItem();
	if (active == nullptr) {return false;}

	if (active->name() != name) {return false;}
	if (! active->isAncientChecked()) {return false;}
	if (! active->isChecked()) {return false;}

	return true;
}
