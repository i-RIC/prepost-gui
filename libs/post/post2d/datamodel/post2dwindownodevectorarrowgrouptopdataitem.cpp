#include "../post2dwindowattributebrowsercontroller.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

Post2dWindowNodeVectorArrowGroupTopDataItem::Post2dWindowNodeVectorArrowGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(resultDataItem()->grid()->vtkData()->data()->GetPointData())) {
		auto item = createChild(val);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}
}

Post2dWindowNodeVectorArrowGroupTopDataItem::~Post2dWindowNodeVectorArrowGroupTopDataItem()
{}

void Post2dWindowNodeVectorArrowGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(static_cast<unsigned int> (m_childItems.size()));
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto di = dynamic_cast<Post2dWindowNodeVectorArrowGroupDataItem*>(item);
		di->update();
	}
}

Post2dWindowCalculationResultDataItem* Post2dWindowNodeVectorArrowGroupTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorArrowGroupTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
	menu->addSeparator();
	menu->addAction(resultDataItem()->nodeScalarGroupTopDataItem()->showAttributeBrowserAction());
}

QDialog* Post2dWindowNodeVectorArrowGroupTopDataItem::addDialog(QWidget* parent)
{
	if (resultDataItem()->grid() == nullptr) {
		return nullptr;
	}

	auto zItem = zoneDataItem();
	auto gType = zItem->v4DataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(zItem->v4DataContainer()->gridData()->grid()->vtkData()->data()->GetPointData())) {
		solutions.insert({sol, gType->outputCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(parent);
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto gType = cont->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = createChild(sol);
	newItem->standardItem()->setText(gType->outputCaption(sol));

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	setZDepthRange(zDepthRange());

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	if (node.nodeName() == "ArrowGroupV4") {
		// old style
		auto target = iRIC::toStr(node.toElement().attribute("target"));
		auto item = createChild(target);
		item->loadFromProjectMainFile(node);
		m_childItems.push_back(item);
	} else if (node.nodeName() == "ArrowGroupTopV4") {
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ArrowGroupV4") {
				auto target = iRIC::toStr(childElem.attribute("target"));
				auto item = createChild(target);
				item->loadFromProjectMainFile(childElem);
				m_childItems.push_back(item);
			}
		}
	}
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowNodeVectorArrowGroupDataItem*> (item);
		writer.writeStartElement("ArrowGroupV4");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

Post2dWindowNodeVectorArrowGroupDataItem* Post2dWindowNodeVectorArrowGroupTopDataItem::createChild(const std::string& name)
{
	auto data = zoneDataItem()->v4DataContainer();
	if (data == nullptr) {return nullptr;}

	auto data2 = data->gridData();
	if (data2 == nullptr) {return nullptr;}

	auto data3 = data2->grid();
	auto unst = dynamic_cast<v4Unstructured2dGrid*> (data3);
	auto st = dynamic_cast<v4Structured2dGrid*> (data3);

	if (st != nullptr) {
		return new Post2dWindowNodeVectorArrowGroupStructuredDataItem(name, this);
	} else if (unst != nullptr) {
		return new Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(name, this);
	} else {
		return nullptr;
	}
}