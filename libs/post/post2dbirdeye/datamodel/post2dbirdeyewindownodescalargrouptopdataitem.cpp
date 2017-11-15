#include "../../../guibase/objectbrowserview.h"
#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/graphicsmisc.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <post/post2d/datamodel/post2dwindowcontoursettingdialog.h>

#include <QAction>
#include <QDomNode>
#include <QList>
#include <QMenu.h>
#include <QMessageBox>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post2dBirdEyeWindowNodeScalarGroupTopDataItem::Post2dBirdEyeWindowNodeScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Contours"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	vtkPointData* pd = cont->data()->GetPointData();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
		m_colorbarTitleMap.insert(name, name.c_str());
	}
}

Post2dBirdEyeWindowNodeScalarGroupTopDataItem::~Post2dBirdEyeWindowNodeScalarGroupTopDataItem()
{
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		Post2dBirdEyeWindowNodeScalarGroupDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowNodeScalarGroupDataItem*>(item);
		typedi->update();
	}
}

QDialog* Post2dBirdEyeWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	if (childItems().size() >= 4) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return nullptr;
	}

	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dBirdEyeWindowZoneDataItem* zItem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer());
	dialog->hideOpacity();
	// region setting
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

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QString s = node.toElement().nodeName();
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
				Post2dBirdEyeWindowNodeScalarGroupDataItem* item = new Post2dBirdEyeWindowNodeScalarGroupDataItem(this);
				item->updateZScale(m_zScale);
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
			Post2dBirdEyeWindowNodeScalarGroupDataItem* item = new Post2dBirdEyeWindowNodeScalarGroupDataItem(this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(node);
			m_childItems.push_back(item);
		}
	}
}

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

class Post2dBirdEyeWindowNodeScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post2dBirdEyeWindowNodeScalarGroupTopDataItem *item, QDialog* propDialog)
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
		m_item = new Post2dBirdEyeWindowNodeScalarGroupDataItem(m_topItem);
		m_item->updateZScale(m_topItem->m_zScale);
		m_topItem->m_childItems.push_back(m_item);
		delete m_undoCommand;
		m_undoCommand = new QUndoCommand();
		m_item->undoCommands(m_propDialog, m_undoCommand);
		m_undoCommand->redo();
		m_topItem->updateItemMap();
	}
	void undo() {
		// don't delete original propDialog
		if (! m_firstDialog) {
			delete m_propDialog;
		}
		m_firstDialog = false;
		m_propDialog = m_item->propertyDialog(nullptr);
		m_undoCommand->undo();
		delete m_item;
		m_item = nullptr;
		m_topItem->updateItemMap();
	}
private:
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem* m_topItem;
	Post2dBirdEyeWindowNodeScalarGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
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

void Post2dBirdEyeWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
