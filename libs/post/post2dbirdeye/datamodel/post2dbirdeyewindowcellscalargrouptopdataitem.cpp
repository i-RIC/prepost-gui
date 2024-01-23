#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowcellscalargroupdataitem.h"
#include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/public/v4grid_attributedataprovider.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/orderedvalueselectdialog.h>
#include <misc/stringtool.h>

Post2dBirdEyeWindowCellScalarGroupTopDataItem::Post2dBirdEyeWindowCellScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Scalar (cell)"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = zoneDataItem()->v4DataContainer();
	auto elevName = cont->gridData()->elevationName(v4SolutionGrid::Position::CellCenter);

	if (elevName.size() != 0) {
		auto item = new Post2dBirdEyeWindowCellScalarGroupDataItem(elevName, this);
		m_childItems.push_back(item);
	}
}

Post2dBirdEyeWindowCellScalarGroupTopDataItem::~Post2dBirdEyeWindowCellScalarGroupTopDataItem()
{}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dBirdEyeWindowCellScalarGroupDataItem*>(item);
		item2->update();
	}
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowCellScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowZoneDataItem*> (parent());
}

QDialog* Post2dBirdEyeWindowCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	if (zItem->v4DataContainer() == nullptr || zItem->v4DataContainer()->gridData() == nullptr) {
		return nullptr;
	}

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->v4DataContainer()->gridData()->grid()->vtkData()->data()->GetCellData());
	auto adProvider = zItem->v4DataContainer()->gridData()->grid()->attributeDataProvider();
	std::unordered_map<std::string, QString> captions;
	for (const auto& sol : solutions) {
		auto c = adProvider->caption(sol);
		captions.insert({sol, c});
	}

	auto dialog = new OrderedValueSelectDialog(p);
	dialog->setValues(solutions, captions);
	dialog->setWindowTitle(tr("Select calculation result to use as Elevation"));

	return dialog;
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post2dBirdEyeWindowCellScalarGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("elevationTarget"));
			auto item = new Post2dBirdEyeWindowCellScalarGroupDataItem(target, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dBirdEyeWindowCellScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("elevationTarget", item2->elevationTarget().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
