#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindownodescalargrouptopdataitem_attributebrowsercontroller.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/orderedvalueselectdialog.h>

Post2dWindowNodeScalarGroupTopDataItem::Post2dWindowNodeScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_showAttributeBrowserAction {new QAction(tr("Show Attribute Browser"), this)},
	m_attributeBrowserController {new AttributeBrowserController {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->gridData()->grid()->vtkData()->data()->GetPointData())) {
		auto item = new Post2dWindowNodeScalarGroupDataItem(val, this);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}

	connect(m_showAttributeBrowserAction, &QAction::triggered, this, &Post2dWindowNodeScalarGroupTopDataItem::showAttributeBrowser);
}

Post2dWindowNodeScalarGroupTopDataItem::~Post2dWindowNodeScalarGroupTopDataItem()
{
	delete m_attributeBrowserController;
}

Post2dWindowAttributeBrowserController* Post2dWindowNodeScalarGroupTopDataItem::attributeBrowserController() const
{
	return m_attributeBrowserController;
}

QAction* Post2dWindowNodeScalarGroupTopDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void Post2dWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post2dWindowNodeScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(static_cast<unsigned int> (m_childItems.size()));
}

void Post2dWindowNodeScalarGroupTopDataItem::update()
{
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		item->update();
	}
}

Post2dWindowCalculationResultDataItem* Post2dWindowNodeScalarGroupTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowNodeScalarGroupTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

QDialog* Post2dWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	if (zItem->v4DataContainer() == nullptr || zItem->v4DataContainer()->gridData() == nullptr) {
		return nullptr;
	}

	auto gType = zItem->v4DataContainer()->gridType();

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->v4DataContainer()->gridData()->grid()->vtkData()->data()->GetPointData());
	std::unordered_map<std::string, QString> captions;
	for (const auto& sol : solutions) {
		auto c = gType->outputCaption(sol);
		captions.insert({sol, c});
	}

	auto dialog = new OrderedValueSelectDialog(p);
	dialog->setValues(solutions, captions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = zoneDataItem()->v4DataContainer();
	if (zoneData == nullptr || zoneData->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post2dWindowNodeScalarGroupDataItem(sol, this);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	setZDepthRange(zDepthRange());

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

bool Post2dWindowNodeScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowNodeScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->initialize();
}

void Post2dWindowNodeScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->clear();
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->fix(event->pos(), v);
}

std::vector<std::string> Post2dWindowNodeScalarGroupTopDataItem::scalarsDrawnInDiscreteMode() const
{
	std::vector<std::string> ret;
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (item->standardItem()->checkState() != Qt::Checked) {continue;}
		if (! item->colorMapIsDiscreteMode()) {continue;}

		ret.push_back(item->target());
	}
	return ret;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkKmlExportCondition(const std::string& target)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->checkKmlExportCondition();
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLHeader(QXmlStreamWriter& writer, const std::string& target)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->exportKMLHeader(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLFooter(QXmlStreamWriter& writer, const std::string& target)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->exportKMLFooter(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, const std::string& target, int index, double time, bool oneShot)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->exportKMLForTimestep(writer, index, time, oneShot);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkShapeExportCondition(const std::string& target)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->colorMapIsDiscreteMode();
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportContourFigureToShape(const std::string& target, const QString& filename, double time)
{
	for (const auto& child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(child);
		if (target == item->target()) {
			return item->exportContourFigureToShape(filename, time);
		}
	}
	return false;
}

void Post2dWindowNodeScalarGroupTopDataItem::showAttributeBrowser()
{
	attributeBrowserController()->initialize();
	auto w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
	menu->addSeparator();
	menu->addAction(m_showAttributeBrowserAction);
}
