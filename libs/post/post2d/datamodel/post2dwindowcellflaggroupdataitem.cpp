#include "post2dwindowcellflagdataitem.h"
#include "post2dwindowcellflaggroupdataitem.h"
#include "post2dwindowcellflagsettingdialog.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"

#include "private/post2dwindowcellflaggroupdataitem_setsettingcommand.h"

#include <guicore/project/colorsource.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributeintegeroptioncell.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QMouseEvent>
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

Post2dWindowCellFlagGroupDataItem::Post2dWindowCellFlagGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Cell attributes"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	initSetting();
}

QDialog* Post2dWindowCellFlagGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowCellFlagSettingDialog* d = new Post2dWindowCellFlagSettingDialog(p);
	SolverDefinitionGridType* gt = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent())->gridType();
	d->setGridType(gt);
	d->setSettings(settings());
	d->setOpacityPercent(m_opacityPercent);
	return d;
}

void Post2dWindowCellFlagGroupDataItem::handlePropertyDialogAccepted(QDialog* d)
{
	Post2dWindowCellFlagSettingDialog* dialog = dynamic_cast<Post2dWindowCellFlagSettingDialog*>(d);
	pushRenderCommand(new SetSettingCommand(dialog->settings(), dialog->opacityPercent(), this), this);
}

QList<Post2dWindowCellFlagSetting> Post2dWindowCellFlagGroupDataItem::settings()
{
	QList<Post2dWindowCellFlagSetting> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
		Post2dWindowCellFlagSetting setting;
		setting.attributeName = item->attributeName();
		setting.value = item->value();
		setting.color = item->color();
		setting.enabled = (item->standardItem()->checkState() == Qt::Checked);
		ret.append(setting);
	}
	return ret;
}

void Post2dWindowCellFlagGroupDataItem::setSettings(const QList<Post2dWindowCellFlagSetting>& settings, int opacity)
{
	QMap<Post2dWindowCellFlagSetting, Post2dWindowCellFlagDataItem*> map;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
		Post2dWindowCellFlagSetting setting;
		setting.attributeName = item->attributeName();
		setting.value = item->value();
		setting.color = item->color();
		setting.enabled  = (item->standardItem()->checkState() == Qt::Checked);
		map.insert(setting, item);
	}
	for (int i = 0; i < m_childItems.size(); ++i) {
		m_standardItem->takeRow(0);
	}
	m_childItems.clear();
	for (auto cit = settings.begin(); cit != settings.end(); ++cit) {
		Post2dWindowCellFlagSetting s = *cit;
		Post2dWindowCellFlagDataItem* item = map.value(s);
		item->setColor(s.color);
		item->setOpacity(opacity);
		if (s.enabled) {
			item->standardItem()->setCheckState(Qt::Checked);
		} else {
			item->standardItem()->setCheckState(Qt::Unchecked);
		}
		item->updateVisibilityWithoutRendering();
		m_standardItem->appendRow(item->standardItem());
		m_childItems.push_back(item);
	}
	m_opacityPercent = opacity;
	updateZDepthRange();
}

void Post2dWindowCellFlagGroupDataItem::initSetting()
{
	ColorSource cs(0);
	cs.load(":/libs/guicore/data/colorsource_cell.xml");

	m_opacityPercent = 50;

	SolverDefinitionGridType* gt = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent())->gridType();
	const QList<SolverDefinitionGridAttribute*>& conds = gt->gridAttributes();
	int index = 0;
	for (auto it = conds.begin(); it != conds.end(); ++it) {
		const SolverDefinitionGridAttribute* cond = *it;
		if (cond->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		if (! cond->isOption()) {continue;}
		const SolverDefinitionGridAttributeInteger* icond = dynamic_cast<const SolverDefinitionGridAttributeInteger*>(cond);
		if (icond == 0) {continue;}

		const IntegerEnumLoader* el = dynamic_cast<const IntegerEnumLoader*>(cond);
		QMap<int, QString> enums = el->enumerations();
		for (auto it = enums.begin(); it != enums.end(); ++it) {
			QColor color = cs.getColor(index ++);
			QString cap = QString("%1 (%2)").arg(icond->caption(), it.value());
			Post2dWindowCellFlagDataItem* item = new Post2dWindowCellFlagDataItem(icond->name(), it.key(), color, cap, this);
			item->setOpacity(m_opacityPercent);
			m_childItems.push_back(item);
		}
	}

	updateItemMap();
	updateZDepthRangeItemCount();
}

void Post2dWindowCellFlagGroupDataItem::update()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
		item->update();
	}
}

void Post2dWindowCellFlagGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_opacityPercent = loadOpacityPercent(node);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
		item->setOpacity(m_opacityPercent);
	}
	QDomNodeList childNodes = node.childNodes();
	for (int i = 0; i < childNodes.count(); ++i) {
		QDomNode cnode = childNodes.at(i);
		QDomElement celem = cnode.toElement();
		std::string attName = iRIC::toStr(celem.attribute("attributeName"));
		int value = celem.attribute("value").toInt();

		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
			if (item->attributeName() == attName && item->value() == value) {
				item->loadFromProjectMainFile(cnode);
			}
		}
	}
}

void Post2dWindowCellFlagGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writeOpacityPercent(m_opacityPercent, writer);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		Post2dWindowCellFlagDataItem* item = dynamic_cast<Post2dWindowCellFlagDataItem*>(*it);
		writer.writeStartElement("CellFlag");
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

bool Post2dWindowCellFlagGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (m_opacityPercent == 100) {return false;}
	for (int i = 0; i < m_childItems.size(); ++i) {
		GraphicsWindowDataItem* item = m_childItems[i];
		if (item->standardItem()->checkState() == Qt::Checked) {return true;}
	}
	return false;
}

void Post2dWindowCellFlagGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initCellInputAttributeBrowser();
}

void Post2dWindowCellFlagGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearCellInputAttributeBrowser();
}

void Post2dWindowCellFlagGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateCellInputAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowCellFlagGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixCellInputAttributeBrowser(QPoint(event->x(), event->y()), v);
}
