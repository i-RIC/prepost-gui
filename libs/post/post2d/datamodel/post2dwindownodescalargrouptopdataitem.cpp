#include "../../../guibase/objectbrowserview.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
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
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post2dWindowNodeScalarGroupTopDataItem::Post2dWindowNodeScalarGroupTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
		m_colorbarTitleMap.insert(name, name.c_str());
	}
}

Post2dWindowNodeScalarGroupTopDataItem::~Post2dWindowNodeScalarGroupTopDataItem()
{
}

void Post2dWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (node.toElement().nodeName() == "Contours") {
		// multi-contours
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarBarTitles") {
				QDomNodeList titles = children.at(i).childNodes();
				for (int j = 0; j < titles.count(); ++j) {
					std::string val = iRIC::toStr(titles.at(j).toElement().attribute("value"));
					QString title = titles.at(j).toElement().attribute("title");
					m_colorbarTitleMap[val] = title;
				}
			} else if (childElem.nodeName() == "ScalarGroup") {
				Post2dWindowNodeScalarGroupDataItem* item = new Post2dWindowNodeScalarGroupDataItem(this);
				item->loadFromProjectMainFile(children.at(i));
				m_childItems.push_back(item);
			}
		}
	}
	else {
		// single-contour (old)
		QDomNodeList titles = node.childNodes();
		for (int i = 0; i < titles.count(); ++i) {
			QDomElement titleElem = titles.at(i).toElement();
			std::string val = iRIC::toStr(titleElem.attribute("value"));
			QString title = titleElem.attribute("title");
			m_colorbarTitleMap[val] = title;
		}
		// only add child if target is non-empty
		Post2dWindowContourSetting setting;
		setting.load(node);
		if (setting.target != "") {
			Post2dWindowNodeScalarGroupDataItem* item = new Post2dWindowNodeScalarGroupDataItem(this);
			item->loadFromProjectMainFile(node);
			m_childItems.push_back(item);
		}
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(m_colorbarTitleMap);
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

void Post2dWindowNodeScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount((unsigned int)m_childItems.size());
}

void Post2dWindowNodeScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->assignActorZValues(range);
	}
}

void Post2dWindowNodeScalarGroupTopDataItem::update()
{
	// forward to children
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		typedi->update();
	}
}

QDialog* Post2dWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	if (childItems().size() >= 4) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return nullptr;
	}

	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dWindowZoneDataItem* zItem = dynamic_cast<Post2dWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer());
	if (! zItem->dataContainer()->IBCExists()) {
		dialog->disableActive();
	}

	Post2dWindowContourSetting setting;
	setting.target = zItem->dataContainer()->data()->GetPointData()->GetArrayName(0);

	switch (childItems().size() % 4) {
	case 0:
		setting.scalarBarSetting.positionX = 0.8;
		setting.scalarBarSetting.positionY = 0.1;
		break;
	case 1:
		setting.scalarBarSetting.positionX = 0.1;
		setting.scalarBarSetting.positionY = 0.1;
		break;
	case 2:
		setting.scalarBarSetting.positionX = 0.1;
		setting.scalarBarSetting.positionY = 0.6;
		break;
	case 3:
		setting.scalarBarSetting.positionX = 0.8;
		setting.scalarBarSetting.positionY = 0.6;
		break;
	}

	dialog->setSetting(setting);
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

bool Post2dWindowNodeScalarGroupTopDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	return true;
}

void Post2dWindowNodeScalarGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeScalarGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarGroupTopDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowNodeScalarGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
	menu->addSeparator();
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

class Post2dWindowNodeScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post2dWindowNodeScalarGroupTopDataItem *item, QDialog* propDialog)
		: QUndoCommand(QObject::tr("Create Contour"))
		, m_topItem(item)
		, m_firstDialog(true)
		, m_propDialog(propDialog)
		, m_undoCommand(nullptr)
	{}
	~CreateCommand() {
		delete m_undoCommand;
		if (! m_firstDialog) {
			delete m_propDialog;
		}
	}
	void redo() {
		m_item = new Post2dWindowNodeScalarGroupDataItem(m_topItem);
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
		if (! m_firstDialog) {
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
	Post2dWindowNodeScalarGroupTopDataItem* m_topItem;
	Post2dWindowNodeScalarGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post2dWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
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

QList<QString> Post2dWindowNodeScalarGroupTopDataItem::selectedScalars()
{
	QList<QString> ret;
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (typedi->standardItem()->checkState() == Qt::Checked) {
			ret.append(typedi->target().c_str());
		}
	}
	return ret;
}

QList<QString> Post2dWindowNodeScalarGroupTopDataItem::availableScalars()
{
	QList<QString> ret;
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		ret.append(typedi->target().c_str());
	}
	return ret;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkKmlExportCondition(const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->checkKmlExportCondition();
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLHeader(QXmlStreamWriter& writer, const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLHeader(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLFooter(QXmlStreamWriter& writer, const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLFooter(writer);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportKMLForTimestep(QXmlStreamWriter& writer, const QString& target, int index, double time)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportKMLForTimestep(writer, index, time);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::exportContourFigureToShape(const QString& target, const QString& filename, double time)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			return typedi->exportContourFigureToShape(filename, time);
		}
	}
	return false;
}

bool Post2dWindowNodeScalarGroupTopDataItem::checkShapeExportCondition(const QString& target)
{
	for (auto item : m_childItems) {
		Post2dWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dWindowNodeScalarGroupDataItem*>(item);
		if (target == QString(typedi->target().c_str())) {
			if (typedi->contour() == ContourSettingWidget::ContourFigure) {
				return true;
			}
		}
	}
	QMessageBox::warning(mainWindow(), tr("Error"), tr("To export shape file, switch \"Display setting\" to \"Contour Figure\", not \"Color Fringe\" or \"Isolines\"."));
	return false;
}
