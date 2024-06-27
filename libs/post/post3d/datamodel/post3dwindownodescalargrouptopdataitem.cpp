#include "../../../guibase/objectbrowserview.h"
#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindownodescalargroupdataitem_impl.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <misc/iricundostack.h>
#include <misc/opacitycontainer.h>
#include <misc/orderedvalueselectdialog.h>

Post3dWindowNodeScalarGroupTopDataItem::Post3dWindowNodeScalarGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Isosurfaces"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

Post3dWindowNodeScalarGroupTopDataItem::~Post3dWindowNodeScalarGroupTopDataItem()
{}

Post3dWindowZoneDataItem* Post3dWindowNodeScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}

void Post3dWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (node.toElement().nodeName() == "Isosurfaces") {
		// multi-isosurface
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarGroup") {
				auto item = new Post3dWindowNodeScalarGroupDataItem(this);
				item->updateZScale(m_zScale);
				item->loadFromProjectMainFile(children.at(i));
				m_childItems.push_back(item);
			}
		}
	} else {
		// single-isosurface
		auto item = new Post3dWindowNodeScalarGroupDataItem(this);
		item->updateZScale(m_zScale);
		item->loadFromProjectMainFile(node);
		m_childItems.push_back(item);
	}
}

void Post3dWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		writer.writeStartElement("ScalarGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowNodeScalarGroupTopDataItem::updateZDepthRangeItemCount()
{}

void Post3dWindowNodeScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& /*range*/)
{}

void Post3dWindowNodeScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowNodeScalarGroupDataItem*>(item);
		child->update();
	}
}

QDialog* Post3dWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	auto gtItem = zItem->gridTypeDataItem();
	auto gType = gtItem->gridType();
	auto cont = zItem->v4DataContainer();

	if (cont == nullptr || cont->gridData() == nullptr) {return nullptr;}

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->gridData()->grid()->vtkData()->data()->GetPointData());

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

void Post3dWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

void Post3dWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post3dWindowNodeScalarGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}

void Post3dWindowNodeScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}
