#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowabstractcellscalargrouptopdataitem.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowabstractcellscalargrouptopdataitem_attributebrowsercontroller.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/base/propertybrowser.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/orderedvalueselectdialog.h>
#include <misc/stringtool.h>

Post2dWindowAbstractCellScalarGroupTopDataItem::Post2dWindowAbstractCellScalarGroupTopDataItem(const QString& caption, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_showAttributeBrowserAction {new QAction(tr("Show Attribute Browser"), this)},
	m_attributeBrowserController {new AttributeBrowserController {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	connect(m_showAttributeBrowserAction, &QAction::triggered, this, &Post2dWindowAbstractCellScalarGroupTopDataItem::showAttributeBrowser);
}

Post2dWindowAbstractCellScalarGroupTopDataItem::~Post2dWindowAbstractCellScalarGroupTopDataItem()
{
	delete m_attributeBrowserController;
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::setupChildren()
{
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cellData())) {
		auto item = createChild(val);
		item->standardItem()->setCheckState(Qt::Unchecked);
		m_childItems.push_back(item);
	}
}

Post2dWindowAttributeBrowserController* Post2dWindowAbstractCellScalarGroupTopDataItem::attributeBrowserController() const
{
	return m_attributeBrowserController;
}

QAction* Post2dWindowAbstractCellScalarGroupTopDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	clearChildItems();

	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement childElem = children.at(i).toElement();
		if (childElem.nodeName() == "ScalarGroup") {
			auto target = iRIC::toStr(childElem.attribute("target"));
			auto item = createChild(target);
			item->loadFromProjectMainFile(childElem);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		auto item2 = dynamic_cast<Post2dWindowAbstractCellScalarGroupDataItem*> (item);
		writer.writeStartElement("ScalarGroup");
		writer.writeAttribute("target", item2->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

vtkCellData* Post2dWindowAbstractCellScalarGroupTopDataItem::cellData() const
{
	return data()->data()->GetCellData();
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto typedi = dynamic_cast<Post2dWindowAbstractCellScalarGroupDataItem*>(item);
		typedi->update();
	}
}

Post2dWindowCalculationResultDataItem* Post2dWindowAbstractCellScalarGroupTopDataItem::resultDataItem() const
{
	return dynamic_cast<Post2dWindowCalculationResultDataItem*> (parent());
}

Post2dWindowZoneDataItem* Post2dWindowAbstractCellScalarGroupTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

QDialog* Post2dWindowAbstractCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	auto zItem = zoneDataItem();
	if (zItem->v4DataContainer() == nullptr || zItem->v4DataContainer()->gridData() == nullptr) {
		return nullptr;
	}

	auto gType = zItem->v4DataContainer()->gridType();

	std::vector<std::string> solutions = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cellData());
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

void Post2dWindowAbstractCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto cont = zoneDataItem()->v4DataContainer();
	if (cont == nullptr || cont->gridData() == nullptr) {
		return;
	}

	auto dialog = dynamic_cast<OrderedValueSelectDialog*> (propDialog);
	auto sol = dialog->selectedValue();

	auto newItem = createChild(sol);

	m_childItems.push_back(newItem);
	updateItemMap();
	iRICUndoStack::instance().clear();

	setZDepthRange(zDepthRange());

	newItem->showPropertyDialog();
}

bool Post2dWindowAbstractCellScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->initialize();
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->clear();
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::showAttributeBrowser()
{
	attributeBrowserController()->initialize();
	auto w = dynamic_cast<Post2dWindow*>(mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowAbstractCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
	menu->addSeparator();
	menu->addAction(m_showAttributeBrowserAction);
}
