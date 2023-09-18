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
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/complex/gridcomplexattributecontainer.h>
#include <guicore/pre/gridcond/container/gridattributeintegercontainer.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroption.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/qwidgetcontainer.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>
#include <misc/xmlsupport.h>

PreProcessorGridAttributeNodeGroupDataItem::PreProcessorGridAttributeNodeGroupDataItem(PreProcessorDataItem* p) :
	PreProcessorDataItem {tr("Node attributes"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()},
	m_opacityWidget {new OpacityContainerWidget(mainWindow())},
	m_colorMapWidgetContainer {new QWidgetContainer(mainWindow())}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	p->standardItem()->takeRow(m_standardItem->row());
	auto typeItem = gridTypeDataItem();
	const auto& conds = typeItem->gridType()->gridAttributes();
	for (auto cond : conds) {
		if (cond->position() == SolverDefinitionGridAttribute::Position::Node) {
			// this is a node condition.
			auto item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert({item->condition()->name(), item});
		}
	}

	const auto& compconds = typeItem->gridType()->gridComplexAttributes();
	for (auto cond : compconds) {
		if (cond->position() == SolverDefinitionGridComplexAttribute::Position::Node) {
			// this is a node condition.
			PreProcessorGridAttributeNodeDataItem* item = new PreProcessorGridAttributeNodeDataItem(cond, this);
			m_childItems.push_back(item);
			m_nameMap.insert({item->condition()->name(), item});
		}
	}

	m_opacity = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(PreProcessorGridAttributeNodeGroupDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));

	m_actor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_actor);

	m_opacityWidget->setContainer(&m_opacity);
	m_opacityWidget->hide();

	connect(m_opacityWidget, &OpacityContainerWidget::updated, [=]() {
		pushUpdateActorSettingCommand(m_opacityWidget->createModifyCommand(), this);
	});

	m_colorMapWidgetContainer->hide();
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
	updateActorSetting();
}

void PreProcessorGridAttributeNodeGroupDataItem::updateActorSetting()
{
	m_opacityWidget->setDisabled(true);
	m_actor->VisibilityOff();

	// make all the items invisible
	m_actorCollection->RemoveAllItems();
	v4InputGrid* g = gridDataItem()->grid();
	if (g == nullptr) {
		// grid is not setup yet.
		return;
	}
	if (m_target == "") {
		updateVisibilityWithoutRendering();
		return;
	}
	m_opacityWidget->setEnabled(true);

	auto cs = gridTypeDataItem()->colorMapSetting(m_target);

	auto grid2d = dynamic_cast<v4Grid2d*> (g->grid());
	auto filteredGrid = grid2d->vtkFilteredData();
	vtkPointData* data = filteredGrid->GetPointData();
	data->SetActiveScalars(m_target.c_str());

	auto mapper = cs->buildPointDataMapper(filteredGrid);
	m_actor->SetMapper(mapper);
	mapper->Delete();

	m_actor->GetProperty()->SetOpacity(m_opacity);

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void PreProcessorGridAttributeNodeGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacity.load(node);
	for (auto item : conditions()) {
		const auto& name = item->condition()->name();
		QDomNode childNode = iRIC::getChildNodeWithAttribute(node, "NodeAttribute", "name", name.c_str());
		if (! childNode.isNull()) {item->loadFromProjectMainFile(childNode);}
	}

	for (auto child: m_childItems) {
		auto item = dynamic_cast<PreProcessorGridAttributeNodeDataItem*> (child);
		if (item->standardItem()->checkState() == Qt::Checked) {
			// this is the current Condition!
			setTarget(item->condition()->name());
		}
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_opacity.save(writer);
	for (auto item : conditions()) {
		writer.writeStartElement("NodeAttribute");
		writer.writeAttribute("name", item->condition()->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::informDataChange(const std::string& name)
{
	gridDataItem()->informGridAttributeChange(name);
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

QDialog* PreProcessorGridAttributeNodeGroupDataItem::propertyDialog(QWidget* parent)
{
	auto child = activeChildItem();
	if (child == nullptr) {return nullptr;}

	return child->propertyDialog(parent);
}

void PreProcessorGridAttributeNodeGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(event->pos(), v);
}

void PreProcessorGridAttributeNodeGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(event->pos(), v);
}

void PreProcessorGridAttributeNodeGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.max());
}

void PreProcessorGridAttributeNodeGroupDataItem::informGridUpdate()
{
	updateActorSetting();
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridAttributeNodeGroupDataItem::activeChildItem() const
{
	return nodeDataItem(m_target);
}

std::vector<PreProcessorGridAttributeNodeDataItem*> PreProcessorGridAttributeNodeGroupDataItem::conditions() const
{
	std::vector<PreProcessorGridAttributeNodeDataItem*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<PreProcessorGridAttributeNodeDataItem*>(child));
	}
	return ret;
}

PreProcessorGridAttributeNodeDataItem* PreProcessorGridAttributeNodeGroupDataItem::nodeDataItem(const std::string& name) const
{
	auto it = m_nameMap.find(name);
	if (it == m_nameMap.end()) {return nullptr;}

	return it->second;
}

void PreProcessorGridAttributeNodeGroupDataItem::handleStandardItemChange()
{
	if (m_isCommandExecuting) {return;}

	iRICUndoStack::instance().beginMacro(QObject::tr("Object Browser Item Change"));
	GraphicsWindowDataItem::handleStandardItemChange();
	if (m_standardItem->checkState() == Qt::Checked) {
		// uncheck other group dataitems
		auto gItem = gridDataItem();
		for (auto child : gItem->childItems()) {
			if (child == this) {continue;}
			if (child == gItem->shapeDataItem()) {continue;}

			child->standardItem()->setCheckState(Qt::Unchecked);
		}
	}
	iRICUndoStack::instance().endMacro();
}

OpacityContainer& PreProcessorGridAttributeNodeGroupDataItem::opacity()
{
	return m_opacity;
}

OpacityContainerWidget* PreProcessorGridAttributeNodeGroupDataItem::opacityWidget() const
{
	return m_opacityWidget;
}

QWidgetContainer* PreProcessorGridAttributeNodeGroupDataItem::colorMapWidgetContainer() const
{
	return m_colorMapWidgetContainer;
}

QAction* PreProcessorGridAttributeNodeGroupDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void PreProcessorGridAttributeNodeGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gridTypeDataItem())) {
		auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gridTypeDataItem())) {
		menu->addAction(m_showAttributeBrowserAction);
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::initAttributeBrowser()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	SolverDefinitionGridType::GridType gt = gridTypeDataItem()->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		pb->view()->resetForVertex(true);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->resetForVertex(false);
	}
}

void PreProcessorGridAttributeNodeGroupDataItem::clearAttributeBrowser()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void PreProcessorGridAttributeNodeGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	auto pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}

	vtkIdType vid = findVertex(p, v);
	m_attributeBrowserFixed = (vid >= 0);
	if (vid < 0) {
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	auto gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	v4InputGrid* grid = gitem->grid();
	grid->grid()->vtkData()->data()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void PreProcessorGridAttributeNodeGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	auto pb = pre->propertyBrowser();
	if (! pb->isVisible()) {return;}
	if (m_attributeBrowserFixed) {return;}

	vtkIdType vid = findVertex(p, v);
	if (vid < 0) {
		pb->view()->resetAttributes();
		return;
	}
	double vertex[3];
	auto gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	v4InputGrid* grid = gitem->grid();
	grid->grid()->vtkData()->data()->GetPoint(vid, vertex);

	updateAttributeBrowser(vid, vertex[0], vertex[1], v);
}

void PreProcessorGridAttributeNodeGroupDataItem::updateAttributeBrowser(vtkIdType vid, double x, double y, VTKGraphicsView* /*v*/)
{
	auto gtItem = gridTypeDataItem();
	auto geoTopItem = gtItem->geoDataTop();
	SolverDefinitionGridType::GridType gt = gtItem->gridType()->defaultGridType();

	auto gitem = dynamic_cast<PreProcessorGridDataItem*>(parent());
	v4InputGrid* grid = gitem->grid();

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
			auto group = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*>
					(geoTopItem->groupDataItem(cAtt->name()));
			if (group->condition()->position() != SolverDefinitionGridComplexAttribute::Position::Node) {continue;}

			auto v = cAtt->value(vid);
			auto g = group->groups().at(v - 1);
			PropertyBrowserAttribute att(group->condition()->caption(), g->caption());
			atts.append(att);
		}
	}
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		auto g2 = dynamic_cast<v4Structured2dGrid*> (gitem->grid()->grid());
		vtkIdType i, j;
		g2->getPointIJIndex(vid, &i, &j);
		pre->propertyBrowser()->view()->setVertexAttributes(i, j, x, y, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pre->propertyBrowser()->view()->setVertexAttributes(vid, x, y, atts);
	}
}

PreProcessorGridTypeDataItem* PreProcessorGridAttributeNodeGroupDataItem::gridTypeDataItem() const
{
	return gridDataItem()->gridTypeDataItem();
}

vtkIdType PreProcessorGridAttributeNodeGroupDataItem::findVertex(const QPoint& p, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<PreProcessorGraphicsView*>(v);
	QPointF pos = v2->viewportToWorld(p);

	v4InputGrid* grid = gridDataItem()->grid();
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
		auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->view()->resetAttributes();
		return -1;
	}

	return vid;
}

bool PreProcessorGridAttributeNodeGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_opacityWidget->setParent(toolBar);
	m_opacityWidget->show();
	toolBar->addWidget(m_opacityWidget);

	toolBar->addSeparator();

	m_colorMapWidgetContainer->setParent(toolBar);
	m_colorMapWidgetContainer->show();
	toolBar->addWidget(m_colorMapWidgetContainer);

	auto activeItem = activeChildItem();
	if (activeItem == 0) {return true;}

	auto grid = gridDataItem()->grid();
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

void PreProcessorGridAttributeNodeGroupDataItem::applyColorMapSetting(const std::string& name)
{
	if (m_target != name) {return;}

	updateActorSetting();
}

PreProcessorGridDataItem* PreProcessorGridAttributeNodeGroupDataItem::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent());
}

bool PreProcessorGridAttributeNodeGroupDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	auto active = activeChildItem();
	if (active == nullptr) {return false;}

	if (active->name() != name) {return false;}
	if (! active->isAncientChecked()) {return false;}
	if (! active->isChecked()) {return false;}

	return true;
}
