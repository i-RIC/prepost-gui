#include "post2dwindowedgeiscalargrouptopdataitem.h"

#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinition.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkCellData.h>
#include <vtkPointData.h>

#include <set>
#include <map>

Post2dWindowEdgeIScalarGroupTopDataItem::Post2dWindowEdgeIScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowScalarGroupTopDataItem{ tr("Scalar (edgeI)"), QIcon(":/libs/guibase/images/iconFolder.png"), p }
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	for (std::string val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->edgeidata()->GetPointData())) {
		colorbarTitleMap().insert(val, val.c_str());
		auto item = new Post2dWindowNodeScalarGroupDataItem(this, NotChecked, NotReorderable, NotDeletable, IFaceCenter);
		m_scalarmap[val] = item;
		m_childItems.push_back(item);
		item->setTarget(val);
		item->m_lookupTableContainer = *(gtItem->nodeLookupTable(val));
	}

}

Post2dWindowEdgeIScalarGroupTopDataItem::~Post2dWindowEdgeIScalarGroupTopDataItem()
{
}

void Post2dWindowEdgeIScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	Q_ASSERT(node.toElement().nodeName() == "ScalarEdgeI");
	if (node.toElement().nodeName() == "ScalarEdgeI") {
		// load contours from main file
		QDomNodeList children = node.childNodes();
		std::set<Post2dWindowNodeScalarGroupDataItem*> missing_quadrant;
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarGroup") {
				std::string solution = iRIC::toStr(children.at(i).toElement().attribute("solution", ""));
				if (solution.size()) {
					auto it = m_scalarmap.find(solution);
					if (it != m_scalarmap.end()) {
						(*it).second->loadFromProjectMainFile(children.at(i));
						// store checked items that have no quadrant set
						if ((*it).second->m_standardItem->checkState() != Qt::Unchecked) {
							if ((*it).second->m_setting.scalarBarSetting.quadrant == ScalarBarSetting::Quadrant::None) {
								missing_quadrant.insert((*it).second);
							}
						}
					}
				}
			}
		}

		Q_ASSERT(missing_quadrant.size() <= 4);
		std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();
		while (missing_quadrant.size() && quads.size()) {
			// find closest item to each quadrant
			auto quad = quads.begin();
			std::multimap<double, Post2dWindowNodeScalarGroupDataItem*> closest;
			for (auto item : missing_quadrant) {
				closest.insert({ item->m_setting.scalarBarSetting.distanceFromDefault(*quad), item });
			}
			closest.begin()->second->m_setting.scalarBarSetting.quadrant = *quad;
			missing_quadrant.erase(closest.begin()->second);
			quads.erase(quad);
		}

		updateItemMap();
		updateVisibilityWithoutRendering();
	}
}

void Post2dWindowEdgeIScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(colorbarTitleMap());
	while (i.hasNext()) {
		i.next();
		if (i.key().size() > 0) {
			writer.writeStartElement("ScalarBarTitle");
			writer.writeAttribute("value", i.key().c_str());
			writer.writeAttribute("title", i.value());
			writer.writeEndElement();
		}
	}
	writer.writeEndElement();

	// contours
	for (auto item : m_childItems) {
		writer.writeStartElement("ScalarGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dWindowEdgeIScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowEdgeIScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->assignActorZValues(range);
	}
}

void Post2dWindowEdgeIScalarGroupTopDataItem::update()
{
	// forward to children
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->update();
	}
}

QDialog* Post2dWindowEdgeIScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer(), IFaceCenter);
	if (! zItem->dataContainer()->IBCExists(IFaceCenter)) {
		dialog->disableActive();
	}

	Post2dWindowContourSetting setting;
	setting.target = zItem->dataContainer()->data(IFaceCenter)->GetPointData()->GetArrayName(0);

	if (! nextScalarBarSetting(setting.scalarBarSetting)) {
		return nullptr;
	}

	dialog->setSetting(setting);
	dialog->setColorBarTitleMap(colorbarTitleMap());

	return dialog;
}

bool Post2dWindowEdgeIScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) { return false; }
	return true;
}

void Post2dWindowEdgeIScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowEdgeIScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowEdgeIScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateEdgeIResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowEdgeIScalarGroupTopDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowEdgeIScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixEdgeIResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowEdgeIScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
	menu->addSeparator();
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

class Post2dWindowEdgeIScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post2dWindowEdgeIScalarGroupTopDataItem *item, QDialog* propDialog)
		: QUndoCommand(QObject::tr("Create Contour"))
		, m_topItem(item)
		, m_firstDialog(true)
		, m_propDialog(propDialog)
		, m_undoCommand(nullptr)
	{}
	~CreateCommand() {
		delete m_undoCommand;
		if (!m_firstDialog) {
			delete m_propDialog;
		}
	}
	void redo() {
		m_item = new Post2dWindowNodeScalarGroupDataItem(m_topItem, Checked, NotReorderable, Deletable, IFaceCenter);
		m_topItem->m_childItems.push_back(m_item);
		delete m_undoCommand;
		m_undoCommand = new QUndoCommand();
		m_item->undoCommands(m_propDialog, m_undoCommand);
		m_undoCommand->redo();
		m_topItem->setZDepthRange(m_topItem->m_zDepthRange);
		m_topItem->updateItemMap();
	}
	void undo() {
		// don't delete original propDialog
		if (!m_firstDialog) {
			delete m_propDialog;
		}
		m_firstDialog = false;
		m_propDialog = m_item->propertyDialog(m_item->mainWindow());
		m_undoCommand->undo();
		delete m_item;
		m_item = nullptr;
		m_topItem->updateItemMap();
	}
private:
	Post2dWindowEdgeIScalarGroupTopDataItem* m_topItem;
	Post2dWindowNodeScalarGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post2dWindowEdgeIScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	iRICUndoStack::instance().push(new CreateCommand(this, propDialog));
	iRICUndoStack::instance().clear();
	//
	// Note: Can't add to stack since m_item is deleted.  The following sequence would
	// cause iRIC to crash:
	// Create new contour -> modify contour -> undo -> undo -> redo -> redo
	// since the modify contour would attempt to use a deleted pointer.
	//
	// Need to be able to remove m_item from m_topItem and reuse m_item.
}
