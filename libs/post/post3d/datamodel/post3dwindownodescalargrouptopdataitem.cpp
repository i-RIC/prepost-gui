#include "../../../guibase/objectbrowserview.h"
#include "../post3dwindowdatamodel.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowisosurfacesettingdialog.h"
#include "post3dwindownodescalargroupdataitem.h"
#include "post3dwindownodescalargrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/datamodel/graphicswindowrootdataitem.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <misc/iricundostack.h>

#include <QAction>
#include <QDomNode>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkRenderer.h>

Post3dWindowNodeScalarGroupTopDataItem::Post3dWindowNodeScalarGroupTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Isosurfaces"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_zScale {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setDefaultValues();
}

Post3dWindowNodeScalarGroupTopDataItem::~Post3dWindowNodeScalarGroupTopDataItem()
{
}

void Post3dWindowNodeScalarGroupTopDataItem::setDefaultValues()
{
	m_isoValue = 0.0;
	m_fullRange = true;
	m_color = Qt::white;
}

void Post3dWindowNodeScalarGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (node.toElement().nodeName() == "Isosurfaces") {
		// multi-isosurface
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "ScalarGroup") {
				auto item = new Post3dWindowNodeScalarGroupDataItem(this);
				item->updateZScale(m_zScale);
				item->loadFromProjectMainFile(children.at(i));
				m_childItems.push_back(item);
			}
		}
	} else {
		// single-isosurface
		auto item = new Post3dWindowNodeScalarGroupDataItem(this);
		item->updateZScale(m_zScale);
		item->loadFromProjectMainFile(node);
		m_childItems.push_back(item);
	}
}

void Post3dWindowNodeScalarGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto item : m_childItems) {
		writer.writeStartElement("ScalarGroup");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post3dWindowNodeScalarGroupTopDataItem::updateZDepthRangeItemCount()
{
}

void Post3dWindowNodeScalarGroupTopDataItem::assignActorZValues(const ZDepthRange& /*range*/)
{
}

void Post3dWindowNodeScalarGroupTopDataItem::update()
{
	for (auto item : m_childItems) {
		auto child = dynamic_cast<Post3dWindowNodeScalarGroupDataItem*>(item);
		child->update();
	}
}

QDialog* Post3dWindowNodeScalarGroupTopDataItem::addDialog(QWidget* p)
{
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent());
	Post3dWindowZoneDataItem* zItem = dynamic_cast<Post3dWindowZoneDataItem*>(parent());

	if (zItem->dataContainer() == nullptr || zItem->dataContainer()->data() == nullptr) {return nullptr;}

	Post3dWindowIsosurfaceSettingDialog* dialog = new Post3dWindowIsosurfaceSettingDialog(p);
	dialog->setGridTypeDataItem(gtItem);


	dialog->setEnabled(true);
	dialog->setZoneData(zItem->dataContainer());
	dialog->setTarget(m_target);

	// it's made enabled ALWAYS.
	//	dialog->setEnabled(isEnabled());
	dialog->setFullRange(m_fullRange);
	dialog->setRange(m_range);

	dialog->setIsoValue(m_isoValue);
	dialog->setColor(this->m_color);

	dialog->setColor(m_color);

	return dialog;
}

void Post3dWindowNodeScalarGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}


class Post3dWindowNodeScalarGroupTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(
		bool enabled, const std::string& sol,
		bool fullrange, StructuredGridRegion::Range3d range,
		double isovalue, const QColor& color, Post3dWindowNodeScalarGroupTopDataItem* topitem)
		: QUndoCommand(QObject::tr("Create Isosurface"))
	{
		m_topItem = topitem;
		m_item = new Post3dWindowNodeScalarGroupDataItem(m_topItem);
		m_topItem->m_childItems.push_back(m_item);

		m_item->setIsCommandExecuting(true);
		m_item->updateZScale(m_topItem->m_zScale);
		m_item->setEnabled(enabled);
		m_item->setTarget(sol);
		m_item->m_fullRange = fullrange;
		m_item->m_range = range;
		m_item->m_isoValue = isovalue;
		m_item->m_color = color;
	}

	void undo()
	{
		m_item->setIsCommandExecuting(true);
		vtkRenderer* r = m_item->renderer();
		r->RemoveActor(m_item->m_isoSurfaceActor);

		QStandardItem* item = m_item->m_standardItem;
		Q_ASSERT(item != nullptr);
		if (item != nullptr) {
			if (item->parent() == nullptr || item->parent()->row() == -1) {
				// maybe this is the top level item of the model
				QStandardItemModel* model = m_item->dataModel()->itemModel();
				QStandardItem* i = model->item(item->row());
				if (i == item) {
					// yes, it is!
					QStandardItem* clone = m_item->standardItem()->clone();
					m_item->dataModel()->itemModel()->removeRow(item->row());
					m_item->m_standardItem = clone;
				}
			} else {
				if (item->parent() != nullptr) {
					QStandardItem* i = item->parent()->child(item->row());
					if (i == item) {
						QStandardItem* clone = m_item->standardItem()->clone();
						item->parent()->removeRow(item->row());
						m_item->m_standardItem = clone;
					}
				}
			}
		}

		m_item->updateActorSettings();
		m_topItem->renderGraphicsView();
		m_item->updateItemMap();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		if (m_item->standardItem()->parent() == nullptr) {
			auto p = dynamic_cast<GraphicsWindowRootDataItem*>(m_topItem);
			if (p == nullptr) {
				auto p2 = dynamic_cast<GraphicsWindowDataItem*>(m_topItem);
				p2->standardItem()->appendRow(m_item->standardItem());
			}
		}

		vtkRenderer* r = m_item->renderer();
		r->AddActor(m_item->m_isoSurfaceActor);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->updateItemMap();
		m_item->setIsCommandExecuting(false);
	}

private:
	Post3dWindowNodeScalarGroupDataItem* m_item;
	Post3dWindowNodeScalarGroupTopDataItem* m_topItem;
};

class Post3dWindowNodeScalarGroupTopDataItem::DeleteCommand : public QUndoCommand
{
public:
	DeleteCommand(
		Post3dWindowNodeScalarGroupTopDataItem* topitem, Post3dWindowNodeScalarGroupDataItem* item)
		: QUndoCommand(QObject::tr("Delete Isosurface"))
	{
		m_topItem = topitem;
		m_item = item;
	}

	void undo()
	{
		m_item->setIsCommandExecuting(true);
		if (m_item->standardItem()->parent() == nullptr) {
			auto p = dynamic_cast<GraphicsWindowRootDataItem*>(m_topItem);
			if (p == nullptr) {
				auto p2 = dynamic_cast<GraphicsWindowDataItem*>(m_topItem);
				p2->standardItem()->appendRow(m_item->standardItem());
			}
		}

		vtkRenderer* r = m_item->renderer();
		r->AddActor(m_item->m_isoSurfaceActor);

		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->updateItemMap();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		vtkRenderer* r = m_item->renderer();
		r->RemoveActor(m_item->m_isoSurfaceActor);

		QStandardItem* item = m_item->m_standardItem;
		Q_ASSERT(item != nullptr);
		if (item != nullptr) {
			if (item->parent() == nullptr || item->parent()->row() == -1) {
				// maybe this is the top level item of the model
				QStandardItemModel* model = m_item->dataModel()->itemModel();
				QStandardItem* i = model->item(item->row());
				if (i == item) {
					// yes, it is!
					QStandardItem* clone = m_item->standardItem()->clone();
					m_item->dataModel()->itemModel()->removeRow(item->row());
					m_item->m_standardItem = clone;
				}
			} else {
				if (item->parent() != nullptr) {
					QStandardItem* i = item->parent()->child(item->row());
					if (i == item) {
						QStandardItem* clone = m_item->standardItem()->clone();
						item->parent()->removeRow(item->row());
						m_item->m_standardItem = clone;
					}
				}
			}
		}

		m_item->updateActorSettings();
		m_topItem->renderGraphicsView();
		m_item->updateItemMap();
		m_item->setIsCommandExecuting(false);
	}

private:
	Post3dWindowNodeScalarGroupDataItem* m_item;
	Post3dWindowNodeScalarGroupTopDataItem* m_topItem;
};


void Post3dWindowNodeScalarGroupTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	Post3dWindowIsosurfaceSettingDialog* dialog = dynamic_cast<Post3dWindowIsosurfaceSettingDialog*>(propDialog);
	iRICUndoStack::instance().push(
		new CreateCommand(
			dialog->enabled(), dialog->target(),
			dialog->fullRange(), dialog->range(),
			dialog->isoValue(), dialog->color(), this)
			);
}

void Post3dWindowNodeScalarGroupTopDataItem::innerUpdateZScale(double scale)
{
	m_zScale = scale;
}

void Post3dWindowNodeScalarGroupTopDataItem::undoableDeleteItem(GraphicsWindowDataItem* item, bool noDraw)
{
	Post3dWindowNodeScalarGroupDataItem* dataitem = dynamic_cast<Post3dWindowNodeScalarGroupDataItem*>(item);
	iRICUndoStack::instance().push(new DeleteCommand(this, dataitem));
}
