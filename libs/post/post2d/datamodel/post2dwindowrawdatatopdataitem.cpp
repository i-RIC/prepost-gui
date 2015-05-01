#include "post2dwindowrawdatagroupdataitem.h"
#include "post2dwindowrawdatatopdataitem.h"

#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <pre/datamodel/preprocessorrawdatatopdataitem.h>

Post2dWindowRawDataTopDataItem::Post2dWindowRawDataTopDataItem(PreProcessorRawDataTopDataItemInterface* ditem, Post2dWindowDataItem* parent)
	: Post2dWindowDataItem(tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItemCopy = m_standardItem->clone();

	m_preRawDataTopDataItem = ditem;

	// add child nodes.
	QList<SolverDefinitionGridRelatedCondition*> list = ditem->gridType()->gridRelatedConditions();
	for (auto it = list.begin(); it != list.end(); ++it) {
		SolverDefinitionGridRelatedCondition* cond = *it;
		QString tmpname = cond->name();
		if (tmpname.toLower().left(9) != "elevation") {continue;}
		Post2dWindowRawDataGroupDataItem* i = new Post2dWindowRawDataGroupDataItem(cond, this);
		m_childItems.append(i);
		m_itemNameMap.insert(cond->name(), i);
	}

	updateChildren();
	connect(ditem, SIGNAL(dataChanged()), this, SLOT(updateChildren()));
}

void Post2dWindowRawDataTopDataItem::updateChildren()
{
	for (int i = 0; i < m_childItems.count(); ++i) {
		Post2dWindowRawDataGroupDataItem* gItem = dynamic_cast<Post2dWindowRawDataGroupDataItem*>(m_childItems.at(i));
		gItem->updateChildren();
	}
	assignActionZValues(m_zDepthRange);
}

void Post2dWindowRawDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		QString name = child.attribute("name");
		Post2dWindowRawDataGroupDataItem* item = m_itemNameMap.value(name);
		if (item != nullptr) {
			item->loadFromProjectMainFile(child);
		}
	}
}

void Post2dWindowRawDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("RawDataGroup");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

