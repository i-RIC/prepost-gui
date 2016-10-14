#include "post3dwindowcontourgrouptopdataitem.h"

#include "post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgroupsettingdialog.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post3dWindowContourGroupTopDataItem::Post3dWindowContourGroupTopDataItem(Post3dWindowDataItem* p) : 
	Post3dWindowDataItem {tr("Contours"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())) {
		m_colorBarTitleMap.insert(name, name.c_str());
	}
}

Post3dWindowContourGroupTopDataItem::~Post3dWindowContourGroupTopDataItem()
{
}

void Post3dWindowContourGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (node.toElement().nodeName() == "Contours") {
		// multi-contours
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarBarTitles") {
				QDomNodeList titles = children.at(i).childNodes();
				for (int i = 0; i < titles.count(); ++i) {
					std::string val = iRIC::toStr(titles.at(i).toElement().attribute("value"));
					QString title = titles.at(i).toElement().attribute("title");
					m_colorBarTitleMap[val] = title;
				}
			} else if (childElem.nodeName() == "ContourGroup") {
				auto item = new Post3dWindowContourGroupDataItem(this);
				item->updateZScale(m_zScale);
				item->loadFromProjectMainFile(children.at(i));
				m_childItems.push_back(item);
			}
		}
	} else {
		// single-contour
		size_t faces = 0;
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarBarTitle") {
				std::string val = iRIC::toStr(childElem.attribute("value"));
				QString title = childElem.attribute("title");
				m_colorBarTitleMap[val] = title;
			} else if (childElem.nodeName() == "FaceSetting") {
				++faces;
			}
		}
		if (faces > 0) {
			auto item  = new Post3dWindowContourGroupDataItem(this);
			item->updateZScale(m_zScale);
			item->loadFromProjectMainFile(node);
			m_childItems.push_back(item);
		}
	}
}

void Post3dWindowContourGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// scalar bar titles
	writer.writeStartElement("ScalarBarTitles");
	QMapIterator<std::string, QString> i(m_colorBarTitleMap);
	while (i.hasNext()) {
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key().c_str());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
	writer.writeEndElement();

	// contours
	for (auto item : m_childItems) {
		writer.writeStartElement("ContourGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowContourGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowContourGroupDataItem*>(item);
		child->update();
	}
}

void Post3dWindowContourGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

QDialog* Post3dWindowContourGroupTopDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowContourGroupSettingDialog* dialog = new Post3dWindowContourGroupSettingDialog(p);
	dialog->setEnabled(true);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* zoneData = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (zoneData == nullptr || zoneData->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zoneData);

	ScalarSettingContainer scalarSetting;
	QMap<QString, Post3dWindowFaceDataItem::Setting> map;

	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent());
	PostZoneDataContainer* c = zItem->dataContainer();
	scalarSetting.target = c->data()->GetPointData()->GetArrayName(0);
	scalarSetting.numberOfDivisions = 10;

	LookupTableContainer *lookupTable = gtItem->nodeLookupTable(iRIC::toStr(scalarSetting.target));
	vtkSmartPointer<vtkScalarBarWidget> scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(scalarBarWidget->GetScalarBarActor());

	dialog->setColorBarTitleMap(m_colorBarTitleMap);
	dialog->setScalarSetting(scalarSetting);
	dialog->setLookupTable(*lookupTable);
	dialog->setFaceMap(map);

	return dialog;
}

class Post3dWindowContourGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post3dWindowContourGroupTopDataItem *item, QDialog* propDialog)
		: QUndoCommand(QObject::tr("Create Contour"))
		, m_topItem(item)
		, m_firstDialog(true)
		, m_propDialog(propDialog)
		, m_undoCommand(nullptr)
	{}
	~CreateCommand() {
		delete m_undoCommand;
		delete m_propDialog;
	}
	void redo() {
		m_item = new Post3dWindowContourGroupDataItem(m_topItem);
		m_item->updateZScale(m_topItem->m_zScale);
		m_topItem->m_childItems.push_back(m_item);
		delete m_undoCommand;
		m_undoCommand = new QUndoCommand();
		m_item->undoCommands(m_propDialog, m_undoCommand);
		m_undoCommand->redo();
	}
	void undo() {
		// don't delete original propDialog
		if (! m_firstDialog) {
			delete m_propDialog;
			m_firstDialog = false;
		}
		m_propDialog = m_item->propertyDialog(nullptr);
		m_undoCommand->undo();
		delete m_item;
		m_item = nullptr;
	}
private:
	Post3dWindowContourGroupTopDataItem* m_topItem;
	Post3dWindowContourGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post3dWindowContourGroupTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	iRICUndoStack::instance().push(new CreateCommand(this, propDialog));
}
