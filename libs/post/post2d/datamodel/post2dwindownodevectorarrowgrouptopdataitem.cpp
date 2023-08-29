#include "post2dwindownodevectorarrowgrouptopdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

Post2dWindowNodeVectorArrowGroupTopDataItem::Post2dWindowNodeVectorArrowGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data(iRICLib::H5CgnsZone::SolutionPosition::Node)->data()->GetPointData())) {
		auto item = buildItem(val);
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

Post2dWindowZoneDataItem* Post2dWindowNodeVectorArrowGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post2dWindowNodeVectorArrowGroupTopDataItem::addDialog(QWidget* parent)
{
	auto zItem = zoneDataItem();
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::Node) == nullptr) {
		return nullptr;
	}

	auto gType = zItem->dataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::Node)->data()->GetPointData())) {
		solutions.insert({sol, gType->outputCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(parent);
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowNodeVectorArrowGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = zoneDataItem()->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = buildItem(sol);
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
		auto item = buildItem(target);
		item->loadFromProjectMainFile(node);
		m_childItems.push_back(item);
	} else if (node.nodeName() == "ArrowGroupTopV4") {
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ArrowGroupV4") {
				auto target = iRIC::toStr(childElem.attribute("target"));
				auto item = buildItem(target);
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

Post2dWindowNodeVectorArrowGroupDataItem* Post2dWindowNodeVectorArrowGroupTopDataItem::buildItem(const std::string& name)
{
	auto data = zoneDataItem()->dataContainer();
	if (data == nullptr) {return nullptr;}

	auto data2 = data->data();
	if (data2 == nullptr) {return nullptr;}

	auto data3 = data2->data();
	auto unst = vtkUnstructuredGrid::SafeDownCast(data3);
	auto st = vtkStructuredGrid::SafeDownCast(data3);

	if (st != nullptr) {
		return new Post2dWindowNodeVectorArrowGroupStructuredDataItem(name, this);
	} else if (unst != nullptr) {
		return new Post2dWindowNodeVectorArrowGroupUnstructuredDataItem(name, this);
	} else {
		return nullptr;
	}
}
