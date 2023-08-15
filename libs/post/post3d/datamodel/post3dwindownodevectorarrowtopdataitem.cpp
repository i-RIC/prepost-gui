#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

Post3dWindowNodeVectorArrowTopDataItem::Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1.0}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->dataContainer();
	auto gType = cont->gridType();

	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data(iRICLib::H5CgnsZone::SolutionPosition::Node)->data()->GetPointData())) {
		auto item = buildItem(val, gType);
		m_childItems.push_back(item);
	}
}

double Post3dWindowNodeVectorArrowTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowNodeVectorArrowTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowNodeVectorArrowTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (item);
		child->update();
	}
}

Post3dWindowZoneDataItem* Post3dWindowNodeVectorArrowTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}

void Post3dWindowNodeVectorArrowTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ArrowGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post3dWindowNodeVectorArrowGroupDataItem(target, this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}
void Post3dWindowNodeVectorArrowTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// arrows
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowNodeVectorArrowGroupDataItem*> (item);
		writer.writeStartElement("ArrowGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowNodeVectorArrowTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post3dWindowNodeVectorArrowTopDataItem::addDialog(QWidget* p)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return nullptr;
	}

	auto gType = zoneData->gridType();

	auto dialog = new ValueSelectDialog(p);
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(zoneData->data()->data()->GetPointData())) {
		solutions.insert({sol, gType->output(sol)->caption()});
	}
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post3dWindowNodeVectorArrowTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = buildItem(sol, gType);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}

Post3dWindowNodeVectorArrowGroupDataItem* Post3dWindowNodeVectorArrowTopDataItem::buildItem(const std::string& name, SolverDefinitionGridType* gtype)
{
	auto newItem = new Post3dWindowNodeVectorArrowGroupDataItem(name, this);
	newItem->standardItem()->setText(gtype->output(name)->caption());
	newItem->updateZScale(m_zScale);

	return newItem;
}
