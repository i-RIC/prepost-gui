#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <postbase/postsolutionselectdialog.h>

Post2dBirdEyeWindowNodeScalarGroupTopDataItem::Post2dBirdEyeWindowNodeScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Scalar (node)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	auto elevName = iRIC::toStr(cont->elevationName());

	if (elevName.size() != 0) {
		auto item = new Post2dBirdEyeWindowNodeScalarGroupDataItem(elevName, this);
		m_childItems.push_back(item);
	}
}

Post2dBirdEyeWindowNodeScalarGroupTopDataItem::~Post2dBirdEyeWindowNodeScalarGroupTopDataItem()
{}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dBirdEyeWindowNodeScalarGroupDataItem*>(item);
		item2->update();
	}
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowNodeScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowZoneDataItem*> (parent());
}

QDialog* Post2dBirdEyeWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		return nullptr;
	}

	auto gType = zItem->dataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->dataContainer()->data()->data()->GetPointData())) {
		solutions.insert({sol, gType->solutionCaption(sol)});
	}

	auto dialog = new PostSolutionSelectDialog(p);
	dialog->setSolutions(solutions);

	return dialog;
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<PostSolutionSelectDialog*> (propDialog);
	auto sol = dialog->selectedSolution();

	auto newItem = new Post2dBirdEyeWindowNodeScalarGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("elevationTarget"));
			auto item = new Post2dBirdEyeWindowNodeScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dBirdEyeWindowNodeScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("elevationTarget", item2->elevationTarget().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
