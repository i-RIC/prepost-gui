#include "../../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindownodescalargroupdataitem.h"
#include "gridbirdeyewindownodescalargrouptopdataitem.h"
#include "gridbirdeyewindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/graphicsmisc.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

GridBirdEyeWindowNodeScalarGroupTopDataItem::GridBirdEyeWindowNodeScalarGroupTopDataItem(GridBirdEyeWindowDataItem* p) :
	GridBirdEyeWindowDataItem {tr("Scalar (node)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	auto gridType = model->gridTypeDataItem()->gridType();
	auto elev = gridType->gridAttribute("Elevation");
	if (elev != nullptr) {
		if (elev->position() == SolverDefinitionGridAttribute::Position::Node) {
			auto item = new GridBirdEyeWindowNodeScalarGroupDataItem("Elevation", this);
			m_childItems.push_back(item);
		}
	}
}

GridBirdEyeWindowNodeScalarGroupTopDataItem::~GridBirdEyeWindowNodeScalarGroupTopDataItem()
{}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<GridBirdEyeWindowNodeScalarGroupDataItem*>(item);
		item2->update();
	}
}

GridBirdEyeWindowZoneDataItem* GridBirdEyeWindowNodeScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<GridBirdEyeWindowZoneDataItem*> (parent());
}

QDialog* GridBirdEyeWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	if (zItem->grid() == nullptr) {
		return nullptr;
	}

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	auto gridType = model->gridTypeDataItem()->gridType();
	std::unordered_map<std::string, QString> attributes;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->grid()->grid()->vtkData()->data()->GetPointData())) {
		attributes.insert({sol, gridType->gridAttributeCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(p);
	dialog->setValues(attributes);
	dialog->setWindowTitle(tr("Select grid attribute to use as Elevation"));

	return dialog;
}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zItem = zoneDataItem();
	if (zItem->grid() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new GridBirdEyeWindowNodeScalarGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("elevationTarget"));
			auto item = new GridBirdEyeWindowNodeScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<GridBirdEyeWindowNodeScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("elevationTarget", item2->elevationTarget().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void GridBirdEyeWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
