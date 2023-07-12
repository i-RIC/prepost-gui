#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valueselectdialog.h>

Post2dWindowNodeScalarGroupTopDataItem::Post2dWindowNodeScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data(iRICLib::H5CgnsZone::SolutionPosition::Node)->data()->GetPointData())) {
		auto item = new Post2dWindowNodeScalarGroupDataItem(val, iRICLib::H5CgnsZone::SolutionPosition::Node, this);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}
}

Post2dWindowNodeScalarGroupTopDataItem::~Post2dWindowNodeScalarGroupTopDataItem()
{}

void Post2dWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = new Post2dWindowNodeScalarGroupDataItem(target, iRICLib::H5CgnsZone::SolutionPosition::Node, this);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowNodeScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->update();
	}
}

Post2dWindowZoneDataItem* Post2dWindowNodeScalarGroupTopDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent());
}

QDialog* Post2dWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::Node) == nullptr) {
		return nullptr;
	}

	auto gType = zItem->dataContainer()->gridType();
	std::unordered_map<std::string, QString> solutions;

	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(zItem->dataContainer()->data(iRICLib::H5CgnsZone::SolutionPosition::Node)->data()->GetPointData())) {
		solutions.insert({sol, gType->solutionCaption(sol)});
	}

	auto dialog = new ValueSelectDialog(p);
	dialog->setValues(solutions);
	dialog->setWindowTitle(tr("Select Calculation Result"));

	return dialog;
}

void Post2dWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto zoneData = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		return;
	}

	auto gType = zoneData->gridType();

	auto dialog = dynamic_cast<ValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = new Post2dWindowNodeScalarGroupDataItem(sol, iRICLib::H5CgnsZone::SolutionPosition::Node, this);
	newItem->standardItem()->setText(gType->solutionCaption(sol));

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	setZDepthRange(zDepthRange());

	dataModel()->objectBrowserView()->select(newItem->standardItem()->index());
	newItem->showPropertyDialog();
}

bool Post2dWindowNodeScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowNodeScalarGroupTopDataItem::informSelection(VTKGraphicsView* v)
{
	zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
}

std::vector<std::string> Post2dWindowNodeScalarGroupTopDataItem::selectedScalars() const
{
	std::vector<std::string> ret;
	for (const auto& item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (typedi->standardItem()->checkState() == Qt::Checked) {
			ret.push_back(typedi->target());
		}
	}
	return ret;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkKmlExportCondition(const std::string& target)
{
	for (const auto& item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == typedi->target()) {
			return typedi->checkKmlExportCondition();
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLHeader(QXmlStreamWriter& writer, const std::string& target)
{
	for (const auto& item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == typedi->target()) {
			return typedi->exportKMLHeader(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLFooter(QXmlStreamWriter& writer, const std::string& target)
{
	for (const auto& item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == typedi->target()) {
			return typedi->exportKMLFooter(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, const std::string& target, int index, double time, bool oneShot)
{
	for (const auto& item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == typedi->target()) {
			return typedi->exportKMLForTimestep(writer, index, time, oneShot);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkShapeExportCondition(const std::string& target)
{
	for (const auto& item : m_childItems) {
		auto scalarItem = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == scalarItem->target()) {
			return scalarItem->checkShapeExportCondition();
		}
	}
	QMessageBox::warning(mainWindow(), tr("Error"), tr("To export shape file, switch color setting to \"Discrete Mode\"."));
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportContourFigureToShape(const std::string& target, const QString& filename, double time)
{
	for (const auto& item : m_childItems) {
		auto scalarItem = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == scalarItem->target()) {
			return scalarItem->exportContourFigureToShape(filename, time);
		}
	}
	return false;
}

void Post2dWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
