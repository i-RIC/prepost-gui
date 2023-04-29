#include "../../../datamodel/preprocessorgridtypedataitem.h"
#include "gridbirdeyewindowcellscalargroupdataitem.h"
#include "gridbirdeyewindowcellscalargrouptopdataitem.h"
#include "gridbirdeyewindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

GridBirdEyeWindowCellScalarGroupTopDataItem::GridBirdEyeWindowCellScalarGroupTopDataItem(GridBirdEyeWindowDataItem* p) :
	GridBirdEyeWindowDataItem {tr("Scalar (cell)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	auto gridType = model->gridTypeDataItem()->gridType();
	auto elev = gridType->gridAttribute("Elevation");
	if (elev != nullptr) {
		if (elev->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
			auto item = new GridBirdEyeWindowCellScalarGroupDataItem("Elevation", this);
			m_childItems.push_back(item);
		}
	}
}

GridBirdEyeWindowCellScalarGroupTopDataItem::~GridBirdEyeWindowCellScalarGroupTopDataItem()
{}

void GridBirdEyeWindowCellScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<GridBirdEyeWindowCellScalarGroupDataItem*>(item);
		item2->update();
	}
}

GridBirdEyeWindowZoneDataItem* GridBirdEyeWindowCellScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<GridBirdEyeWindowZoneDataItem*> (parent());
}

QDialog* GridBirdEyeWindowCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	if (zItem->grid() == nullptr || zItem->grid()->vtkGrid() == nullptr) {
		return nullptr;
	}

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (dataModel());
	auto gridType = model->gridTypeDataItem()->gridType();
	std::unordered_map<std::string, QString> attributes;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->grid()->vtkGrid()->GetCellData())) {
		attributes.insert({sol, gridType->gridAttributeCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(p);
	dialog->setValues(attributes);
	dialog->setWindowTitle(tr("Select Grid Attribute"));

	return dialog;
}

void GridBirdEyeWindowCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zItem = zoneDataItem();
	if (zItem->grid() == nullptr || zItem->grid()->vtkGrid() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new GridBirdEyeWindowCellScalarGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

void GridBirdEyeWindowCellScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void GridBirdEyeWindowCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("elevationTarget"));
			auto item = new GridBirdEyeWindowCellScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void GridBirdEyeWindowCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<GridBirdEyeWindowCellScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("elevationTarget", item2->elevationTarget().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void GridBirdEyeWindowCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
