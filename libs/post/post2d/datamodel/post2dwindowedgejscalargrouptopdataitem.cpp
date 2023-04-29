#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowedgejscalargrouptopdataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

#include <QDomNode>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post2dWindowEdgeJScalarGroupTopDataItem::Post2dWindowEdgeJScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar (edgeJ)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data(iRICLib::H5CgnsZone::SolutionPosition::JFace)->data()->GetPointData())) {
		auto item = new Post2dWindowNodeScalarGroupDataItem(val, iRICLib::H5CgnsZone::SolutionPosition::JFace, this);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}
}

Post2dWindowEdgeJScalarGroupTopDataItem::~Post2dWindowEdgeJScalarGroupTopDataItem()
{}

void Post2dWindowEdgeJScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post2dWindowNodeScalarGroupDataItem(target, iRICLib::H5CgnsZone::SolutionPosition::JFace, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowEdgeJScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowEdgeJScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowEdgeJScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	for (auto item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->assignActorZValues(range);
	}
}

void Post2dWindowEdgeJScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->update();
	}
}

Post2dWindowZoneDataItem* Post2dWindowEdgeJScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

QDialog* Post2dWindowEdgeJScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::JFace) == nullptr) {
		return nullptr;
	}

	auto gType = zItem->dataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::JFace)->data()->GetPointData())) {
		solutions.insert({sol, gType->solutionCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(p);
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowEdgeJScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post2dWindowNodeScalarGroupDataItem(sol, iRICLib::H5CgnsZone::SolutionPosition::JFace, this);
	newItem->standardItem()->setText(gType->solutionCaption(sol));

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}

bool Post2dWindowEdgeJScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowEdgeJScalarGroupTopDataItem::informSelection(VTKGraphicsView* v)
{
	zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowEdgeJScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowEdgeJScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateEdgeJResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowEdgeJScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixEdgeJResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowEdgeJScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
