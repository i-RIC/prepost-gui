#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowcellscalargrouptopdataitem.h"
#include "post2dwindowcellscalargroupdataitem.h"
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

#include <vtkCellData.h>

Post2dWindowCellScalarGroupTopDataItem::Post2dWindowCellScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar (cell center)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->data()->GetCellData())) {
		if (PostZoneDataContainer::hasInputDataPrefix(val)) {continue;}

		auto item = new Post2dWindowCellScalarGroupDataItem(val, this);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}
}

Post2dWindowCellScalarGroupTopDataItem::~Post2dWindowCellScalarGroupTopDataItem()
{}

void Post2dWindowCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post2dWindowCellScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowCellScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowCellScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowCellScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowCellScalarGroupDataItem*>(item);
		typedi->update();
	}
}

Post2dWindowZoneDataItem* Post2dWindowCellScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

QDialog* Post2dWindowCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		return nullptr;
	}

	auto gType = zItem->dataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->dataContainer()->data()->data()->GetCellData())) {
		if (PostZoneDataContainer::hasInputDataPrefix(sol)) {continue;}
		solutions.insert({sol, gType->solutionCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(p);
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post2dWindowCellScalarGroupDataItem(sol, this);
	newItem->standardItem()->setText(gType->solutionCaption(sol));

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	setZDepthRange(zDepthRange());

	newItem->showPropertyDialog();
}

bool Post2dWindowCellScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowCellScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->initCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearCellResultAttributeBrowser();
}

void Post2dWindowCellScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateCellResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowCellScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixCellResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
