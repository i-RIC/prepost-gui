#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <postbase/postsolutionselectdialog.h>

Post3dWindowContourGroupTopDataItem::Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Contours"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

double Post3dWindowContourGroupTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowContourGroupDataItem*> (item);
		child->update();
	}
}

void Post3dWindowContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ContourGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post3dWindowContourGroupDataItem(target, this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// contours
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowContourGroupDataItem*> (item);
		writer.writeStartElement("ContourGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowContourGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post3dWindowContourGroupTopDataItem::addDialog(QWidget* p)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return nullptr;
	}

	auto gType = zoneData->gridType();

	auto dialog = new PostSolutionSelectDialog(p);
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zoneData->data()->data()->GetPointData())) {
		solutions.insert({sol, gType->solutionCaption(sol)});
	}
	dialog->setSolutions(solutions);

	return dialog;
}

void Post3dWindowContourGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<PostSolutionSelectDialog*> (propDialog);
	auto sol = dialog->selectedSolution();

	auto newItem = new Post3dWindowContourGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}
