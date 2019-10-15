#include "../../../guibase/objectbrowserview.h"
#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowcellscalargroupdataitem.h"
#include "post2dbirdeyewindowcellscalargrouptopdataitem.h"
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

#include <vtkCellData.h>
#include <vtkPointData.h>

Post2dBirdEyeWindowCellScalarGroupTopDataItem::Post2dBirdEyeWindowCellScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Scalar (cell center)"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	vtkCellData* cd = cont->data()->GetCellData();
	for (const auto& val : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd)) {
		if (PostZoneDataContainer::hasInputDataPrefix(val)) {continue;}

		m_colorbarTitleMap.insert(val, val.c_str());
		auto item = new Post2dBirdEyeWindowCellScalarGroupDataItem(this, NotChecked, NotReorderable, NotDeletable);
		m_scalarmap[val] = item;
		m_childItems.push_back(item);
		item->setTarget(val);
	}
}

Post2dBirdEyeWindowCellScalarGroupTopDataItem::~Post2dBirdEyeWindowCellScalarGroupTopDataItem()
{
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::update()
{
	// update children
	for (auto item : m_childItems) {
		Post2dBirdEyeWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowCellScalarGroupDataItem*>(item);
		typedi->update();
	}
}

QDialog* Post2dBirdEyeWindowCellScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post2dWindowContourSettingDialog* dialog = new Post2dWindowContourSettingDialog(p);
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	Post2dBirdEyeWindowZoneDataItem* zItem = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent());
	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {
		delete dialog;
		return nullptr;
	}
	dialog->setZoneData(zItem->dataContainer(), CellCenter);
	dialog->hideOpacity();
	// region setting
	if (! zItem->dataContainer()->IBCCellExists()) {
		dialog->disableActive();
	}

	Post2dWindowContourSetting setting;
	setting.target = zItem->dataContainer()->data()->GetPointData()->GetArrayName(0);

	if (!nextScalarBarSetting(setting.scalarBarSetting)) {
		return nullptr;
	}

	dialog->setSetting(setting);
	dialog->setColorBarTitleMap(m_colorbarTitleMap);

	return dialog;
}

bool Post2dBirdEyeWindowCellScalarGroupTopDataItem::nextScalarBarSetting(ScalarBarSetting& scalarBarSetting)
{
	std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();

	for (auto item : m_childItems) {
		Post2dBirdEyeWindowCellScalarGroupDataItem* typedi = dynamic_cast<Post2dBirdEyeWindowCellScalarGroupDataItem*>(item);
		// note use m_standardItemCopy which hasn't been changed yet
		if (typedi->m_standardItemCopy->checkState() == Qt::Checked) {
			auto it = quads.find(typedi->m_setting.scalarBarSetting.quadrant);
			if (it != quads.end()) {
				quads.erase(it);
			}
		}
	}
	if (quads.empty()) {
		QMessageBox::warning(postProcessorWindow(), tr("Warning"), tr("A maximum of four contours may be defined."));
		return false;
	}
	scalarBarSetting.setDefaultPosition(*quads.begin());
	return true;
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QString s = node.toElement().nodeName();
	if (node.toElement().nodeName() == "Contours") {
		// multi-contours
		QDomNodeList children = node.childNodes();
		std::set<Post2dBirdEyeWindowCellScalarGroupDataItem*> missing_quadrant;
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
				std::string solution = iRIC::toStr(children.at(i).toElement().attribute("solution", ""));
				if (solution.size()) {
					auto it = m_scalarmap.find(solution);
					Q_ASSERT(it != m_scalarmap.end());
					if (it != m_scalarmap.end()) {
						(*it).second->updateZScale(m_zScale);
						(*it).second->loadFromProjectMainFile(children.at(i));
						// store checked items that have no quadrant set
						if ((*it).second->m_standardItem->checkState() != Qt::Unchecked) {
							if ((*it).second->m_setting.scalarBarSetting.quadrant == ScalarBarSetting::Quadrant::None) {
								missing_quadrant.insert((*it).second);
							}
						}
					}
				}

				Q_ASSERT(missing_quadrant.size() <= 4);
				std::set<ScalarBarSetting::Quadrant> quads = ScalarBarSetting::getQuadrantSet();
				while (missing_quadrant.size() && quads.size()) {
					// find closest item to each quadrant
					auto quad = quads.begin();
					std::multimap<double, Post2dBirdEyeWindowCellScalarGroupDataItem*> closest;
					for (auto item : missing_quadrant) {
						closest.insert({ item->m_setting.scalarBarSetting.distanceFromDefault(*quad), item });
					}
					closest.begin()->second->m_setting.scalarBarSetting.quadrant = *quad;
					missing_quadrant.erase(closest.begin()->second);
					quads.erase(quad);
				}
			}
		}

	}
	else {
		// single-contour (old)

		// only add child if target is non-empty
		Post2dWindowContourSetting setting;
		setting.load(node);
		if (setting.target != "") {
			auto it = m_scalarmap.find(setting.target);
			if (it != m_scalarmap.end()) {
				(*it).second->updateZScale(m_zScale);
				(*it).second->loadFromProjectMainFile(node);
				(*it).second->m_setting.scalarBarSetting.quadrant = ScalarBarSetting::Quadrant::RightLower;
			}
		}
	}
}

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

class Post2dBirdEyeWindowCellScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(Post2dBirdEyeWindowCellScalarGroupTopDataItem *item, QDialog* propDialog)
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
		m_item = new Post2dBirdEyeWindowCellScalarGroupDataItem(m_topItem, Checked, NotReorderable, Deletable);
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
	Post2dBirdEyeWindowCellScalarGroupTopDataItem* m_topItem;
	Post2dBirdEyeWindowCellScalarGroupDataItem* m_item;
	QDialog* m_propDialog;
	QUndoCommand* m_undoCommand;
	bool m_firstDialog;
};

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
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

void Post2dBirdEyeWindowCellScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}
