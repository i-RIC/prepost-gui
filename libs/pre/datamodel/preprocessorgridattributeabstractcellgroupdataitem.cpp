#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "../preprocessorwindow.h"
#include "attributebrowserhelper.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgridattributeabstractcelldataitem.h"
#include "preprocessorgridattributeabstractcellgroupdataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgridtypedataitem.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/propertybrowserattribute.h>
#include <guicore/datamodel/propertybrowserview.h>
#include <guicore/grid/v4grid2d.h>
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

PreProcessorGridAttributeAbstractCellGroupDataItem::PreProcessorGridAttributeAbstractCellGroupDataItem(const QString& caption, PreProcessorDataItem* p) :
	PreProcessorDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_actor {vtkActor::New()},
	m_lineWidth {"lineWidth", 5},
	m_opacityWidget {new OpacityContainerWidget(mainWindow())},
	m_colorMapWidgetContainer {new QWidgetContainer(mainWindow())}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	m_opacity = 50;
	m_attributeBrowserFixed = false;

	m_showAttributeBrowserAction = new QAction(PreProcessorGridAttributeAbstractCellGroupDataItem::tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));

	m_actor->GetProperty()->SetLighting(false);
	m_actor->GetProperty()->SetLineWidth(5);
	renderer()->AddActor(m_actor);

	m_opacityWidget->setContainer(&m_opacity);
	m_opacityWidget->hide();

	connect(m_opacityWidget, &OpacityContainerWidget::updated, [=]() {
			pushUpdateActorSettingCommand(m_opacityWidget->createModifyCommand(), this);
	});

	m_colorMapWidgetContainer->hide();
}

PreProcessorGridAttributeAbstractCellGroupDataItem::~PreProcessorGridAttributeAbstractCellGroupDataItem()
{
	renderer()->RemoveActor(m_actor);

	m_actor->Delete();
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::setupChildren()
{
	auto pos = definitionPosition();

	auto p = dynamic_cast<PreProcessorDataItem*> (parent());
	p->standardItem()->takeRow(m_standardItem->row());
	auto typeItem = gridTypeDataItem();
	const auto& conds = typeItem->gridType()->gridAttributes();
	for (auto cond : conds) {
		if (cond->position() == pos) {
			auto item = createChild(cond);
			m_childItems.push_back(item);
			m_nameMap.insert({item->condition()->name(), item});
		}
	}
	const auto& compConds = typeItem->gridType()->gridComplexAttributes();
	for (auto cond : compConds) {
		if (cond->position() == pos) {
			auto item = createChild(cond);
			m_childItems.push_back(item);
			m_nameMap.insert({item->condition()->name(), item});
		}
	}
}

std::string PreProcessorGridAttributeAbstractCellGroupDataItem::target() const
{
	return m_target;
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSetting();
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Cell Attribute Change"));
	pushRenderCommand(cmd, this, true);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::updateActorSetting()
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

	auto cond = activeChildItem()->condition();
	if (cond->isDirection()) {
		// delegate to child
		activeChildItem()->updateActorSetting();
	} else {
		auto cs = gridTypeDataItem()->colorMapSetting(m_target);

		auto filteredGrid = filteredData();
		vtkCellData* data = filteredGrid->GetCellData();
		data->SetActiveScalars(m_target.c_str());

		auto mapper = cs->buildCellDataMapper(filteredGrid, false);
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetOpacity(m_opacity);
		m_actor->GetProperty()->SetLineWidth(m_lineWidth);

		m_actorCollection->AddItem(m_actor);
		updateVisibilityWithoutRendering();
	}
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::informDataChange(const std::string& name)
{
	gridDataItem()->informGridAttributeChange(name);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	initAttributeBrowser();
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	clearAttributeBrowser();
}

QDialog* PreProcessorGridAttributeAbstractCellGroupDataItem::propertyDialog(QWidget* parent)
{
	auto child = activeChildItem();
	if (child == nullptr) {return nullptr;}

	return child->propertyDialog(parent);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	updateAttributeBrowser(event->pos(), v);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	fixAttributeBrowser(event->pos(), v);
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::informGridUpdate()
{
	updateActorSetting();
}

PreProcessorGridAttributeAbstractCellDataItem* PreProcessorGridAttributeAbstractCellGroupDataItem::activeChildItem() const
{
	return cellDataItem(m_target);
}

std::vector<PreProcessorGridAttributeAbstractCellDataItem*> PreProcessorGridAttributeAbstractCellGroupDataItem::conditions() const
{
	std::vector<PreProcessorGridAttributeAbstractCellDataItem*> ret;
	for (auto child : m_childItems) {
		ret.push_back(dynamic_cast<PreProcessorGridAttributeAbstractCellDataItem*>(child));
	}
	return ret;
}

PreProcessorGridAttributeAbstractCellDataItem* PreProcessorGridAttributeAbstractCellGroupDataItem::cellDataItem(const std::string& name) const
{
	auto it = m_nameMap.find(name);
	if (it == m_nameMap.end()) {return nullptr;}

	return it->second;
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::handleStandardItemChange()
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

IntContainer& PreProcessorGridAttributeAbstractCellGroupDataItem::lineWidth()
{
	return m_lineWidth;
}

OpacityContainer& PreProcessorGridAttributeAbstractCellGroupDataItem::opacity()
{
	return m_opacity;
}

OpacityContainerWidget* PreProcessorGridAttributeAbstractCellGroupDataItem::opacityWidget() const
{
	return m_opacityWidget;
}

QWidgetContainer* PreProcessorGridAttributeAbstractCellGroupDataItem::colorMapWidgetContainer() const
{
	return m_colorMapWidgetContainer;
}

QAction* PreProcessorGridAttributeAbstractCellGroupDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::showAttributeBrowser()
{
	initAttributeBrowser();
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gridTypeDataItem())) {
		auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
		pre->propertyBrowser()->show();
	}
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	if (AttributeBrowserHelper::isAttributeBrowserAvailable(gridTypeDataItem())){
		menu->addAction(m_showAttributeBrowserAction);
	}
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::initAttributeBrowser()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	SolverDefinitionGridType::GridType gt = gridTypeDataItem()->gridType()->defaultGridType();
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		pb->view()->resetForCell(true);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pb->view()->resetForCell(false);
	}
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::clearAttributeBrowser()
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	PropertyBrowser* pb = pre->propertyBrowser();
	pb->view()->hideAll();
	m_attributeBrowserFixed = false;
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::fixAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
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

void PreProcessorGridAttributeAbstractCellGroupDataItem::updateAttributeBrowser(const QPoint& p, VTKGraphicsView* v)
{
	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
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

void PreProcessorGridAttributeAbstractCellGroupDataItem::updateAttributeBrowser(vtkIdType cellid, VTKGraphicsView* /*v*/)
{
	auto gItem = gridDataItem();
	v4InputGrid* grid = gItem->grid();

	auto gtItem = gridTypeDataItem();
	auto geoTopItem = gtItem->geoDataTop();
	SolverDefinitionGridType::GridType gt = gtItem->gridType()->defaultGridType();

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
			auto group = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemI*>
							(geoTopItem->groupDataItem(cAtt->name()));
			if (group->condition()->position() != pos) {continue;}

			auto v = cAtt->value(cellid);
			if (v == 0) {v = 1;} // in case not mapped yet, change value to Default (= 1)
			auto g = group->groups().at(v - 1);
			PropertyBrowserAttribute att(group->condition()->caption(), g->caption());
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

	auto pre = dynamic_cast<PreProcessorWindow*>(preProcessorWindow());
	if (gt == SolverDefinitionGridType::gtStructured2DGrid) {
		vtkIdType i, j;
		getIJIndex(cellid, &i, &j);
		pre->propertyBrowser()->view()->setCellAttributes(i, j, polygon, atts);
	} else if (gt == SolverDefinitionGridType::gtUnstructured2DGrid) {
		pre->propertyBrowser()->view()->setCellAttributes(cellid, polygon, atts);
	}
}

PreProcessorGridTypeDataItem* PreProcessorGridAttributeAbstractCellGroupDataItem::gridTypeDataItem() const
{
	return gridDataItem()->gridTypeDataItem();
}

vtkIdType PreProcessorGridAttributeAbstractCellGroupDataItem::findCell(const QPoint& p, VTKGraphicsView* v)
{
	auto v2 = dynamic_cast<PreProcessorGraphicsView*>(v);
	QPointF pos = v2->viewportToWorld(p);

	auto gItem = gridDataItem();
	v4InputGrid* grid = gItem->grid();
	if (grid == nullptr) {
		return -1;
	}

	double weights[4];
	return data()->findCell(pos.x(), pos.y(), 0, v2->stdRadius(iRIC::nearRadius()), weights);
}

bool PreProcessorGridAttributeAbstractCellGroupDataItem::addToolBarButtons(QToolBar* toolBar)
{
	m_opacityWidget->setParent(toolBar);
	m_opacityWidget->show();
	toolBar->addWidget(m_opacityWidget);

	toolBar->addSeparator();

	m_colorMapWidgetContainer->setParent(toolBar);
	m_colorMapWidgetContainer->show();
	toolBar->addWidget(m_colorMapWidgetContainer);

	auto activeItem = activeChildItem();
	if (activeItem == nullptr) {return true;}

	auto grid = gridDataItem()->grid();
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

void PreProcessorGridAttributeAbstractCellGroupDataItem::applyColorMapSetting(const std::string& name)
{
	if (m_target != name) {return;}

	updateActorSetting();
}

void PreProcessorGridAttributeAbstractCellGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacity.load(node);
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

void PreProcessorGridAttributeAbstractCellGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_opacity.save(writer);
	for (auto item : conditions()) {
		writer.writeStartElement("CellAttribute");
		writer.writeAttribute("name", item->condition()->name().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

PreProcessorGridDataItem* PreProcessorGridAttributeAbstractCellGroupDataItem::gridDataItem() const
{
	return dynamic_cast<PreProcessorGridDataItem*> (parent());
}

bool PreProcessorGridAttributeAbstractCellGroupDataItem::colorBarShouldBeVisible(const std::string& name) const
{
	auto active = activeChildItem();
	if (active == nullptr) {return false;}

	if (active->name() != name) {return false;}
	if (! active->isAncientChecked()) {return false;}
	if (! active->isChecked()) {return false;}

	return true;
}
