#include "post3dwindowfacecontourgroupdataitem.h"
#include "post3dwindowfacecontourgrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>
#include <guicore/grid/v4grid.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/orderedvalueselectdialog.h>

Post3dWindowFaceContourGroupTopDataItem::Post3dWindowFaceContourGroupTopDataItem(const QString& caption, v4SolutionGrid::Position pos, Post3dWindowDataItem* p) :
	Post3dWindowDataItem(caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p),
	m_zScale {1},
	m_position {pos}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

double Post3dWindowFaceContourGroupTopDataItem::zScale() const
{
	return m_zScale;
}

void Post3dWindowFaceContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowFaceContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowFaceContourGroupDataItem*> (item);
		child->update();
	}
}


v4SolutionGrid::Position Post3dWindowFaceContourGroupTopDataItem::position() const
{
	return m_position;
}

void Post3dWindowFaceContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ContourGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post3dWindowFaceContourGroupDataItem(target, this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(children.at(i));
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowFaceContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// contours
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post3dWindowFaceContourGroupDataItem*> (item);
		writer.writeStartElement("ContourGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowFaceContourGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post3dWindowFaceContourGroupTopDataItem::addDialog(QWidget* p)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return nullptr;
	}

	auto gType = cont->gridType();

	auto sGrid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid());
	vtkPolyDataExtended3d* pd = nullptr;
	if (m_position == v4SolutionGrid::Position::IFace) {
		pd = sGrid->vtkIFaceData();
	} else if (m_position == v4SolutionGrid::Position::JFace) {
		pd = sGrid->vtkJFaceData();
	} else if (m_position == v4SolutionGrid::Position::KFace) {
		pd = sGrid->vtkKFaceData();
	}
	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd->data()->GetCellData());
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

void Post3dWindowFaceContourGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post3dWindowFaceContourGroupDataItem(sol, this);
	newItem->updateZScale(m_zScale);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	newItem->showPropertyDialog();
}

Post3dWindowZoneDataItem* Post3dWindowFaceContourGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post3dWindowZoneDataItem*> (parent());
}
