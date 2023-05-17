#include "post3dwindowcellcontourgroupdataitem.h"
#include "post3dwindowcellcontourgrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

Post3dWindowCellContourGroupTopDataItem::Post3dWindowCellContourGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Contours (cell center)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

double Post3dWindowCellContourGroupTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowCellContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowCellContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowCellContourGroupDataItem*> (item);
		child->update();
	}
}

void Post3dWindowCellContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ContourGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post3dWindowCellContourGroupDataItem(target, this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowCellContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// contours
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowCellContourGroupDataItem*> (item);
		writer.writeStartElement("ContourGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowCellContourGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post3dWindowCellContourGroupTopDataItem::addDialog(QWidget* p)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return nullptr;
	}

	auto gType = zoneData->gridType();

	auto dialog = new ValueSelectDialog(p);
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zoneData->data()->data()->GetCellData())) {
		solutions.insert({sol, gType->output(sol)->caption()});
	}
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));
	return dialog;
}

void Post3dWindowCellContourGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post3dWindowCellContourGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}
