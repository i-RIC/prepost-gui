#include "post3dwindownodevectorarrowgroupdataitem.h"
#include "post3dwindownodevectorarrowtopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/orderedvalueselectdialog.h>

Post3dWindowNodeVectorArrowTopDataItem::Post3dWindowNodeVectorArrowTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1.0}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	auto gType = cont->gridType();

	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->gridData()->grid()->vtkData()->data()->GetPointData())) {
		auto item = buildItem(val);
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
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return nullptr;
	}

	auto gType = cont->gridType();

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->gridData()->grid()->vtkData()->data()->GetPointData());
	std::unordered_map<std::string, QString> captions;
	for (const auto& sol : solutions) {
		auto c = gType->vectorOutputCaption(sol);
		captions.insert({sol, c});
	}

	auto dialog = new OrderedValueSelectDialog(p);
	dialog->setValues(solutions, captions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post3dWindowNodeVectorArrowTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto gType = cont->gridType();

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = buildItem(sol);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}

Post3dWindowNodeVectorArrowGroupDataItem* Post3dWindowNodeVectorArrowTopDataItem::buildItem(const std::string& name)
{
	auto newItem = new Post3dWindowNodeVectorArrowGroupDataItem(name, this);
	newItem->updateZScale(m_zScale);

	return newItem;
}
