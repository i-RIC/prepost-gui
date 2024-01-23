#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/orderedvalueselectdialog.h>

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

Post3dWindowZoneDataItem* Post3dWindowContourGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
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
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post3dWindowContourGroupDataItem*> (child);
		writer.writeStartElement("ContourGroup");
		writer.writeAttribute("target", item->target().c_str());
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
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return nullptr;
	}

	auto gType = cont->gridType();

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->gridData()->grid()->vtkData()->data()->GetPointData());
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

void Post3dWindowContourGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post3dWindowContourGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}
