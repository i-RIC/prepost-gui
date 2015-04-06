#include "graph2dscatteredwindowrootdataitem.h"
#include "../graph2dscatteredwindowdatamodel.h"
#include "../graph2dscatteredwindow.h"
#include "graph2dscatteredwindowresultgroupdataitem.h"
#include <misc/xmlsupport.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include "../graph2dscatteredwindowview.h"
#include "../graph2dscatteredwindowdatamodel.h"
#include <guicore/postcontainer/postsolutioninfo.h>
#include "graph2dscatteredwindowresultgroupdataitem.h"

#include <QStandardItemModel>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Graph2dScatteredWindowRootDataItem::Graph2dScatteredWindowRootDataItem(Graph2dScatteredWindow* window, ProjectDataItem* parent)
	: Graph2dWindowRootDataItem(window, parent)
{
	m_resultGroupItem = new Graph2dScatteredWindowResultGroupDataItem(this);
	m_childItems.append(m_resultGroupItem);
	updateItemMap();
}

Graph2dScatteredWindowRootDataItem::~Graph2dScatteredWindowRootDataItem()
{
	delete m_resultGroupItem;
}

void Graph2dScatteredWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();
	model->appendRow(m_resultGroupItem->standardItem());
	emit standardModelSetuped();
}

void Graph2dScatteredWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode resultNode = iRIC::getChildNode(node, "CalculationResult");
	if (! resultNode.isNull()){
		m_resultGroupItem->loadFromProjectMainFile(resultNode);
	}
	updateItemMap();
}
void Graph2dScatteredWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("CalculationResult");
	m_resultGroupItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Graph2dScatteredWindowRootDataItem::updateData(int fn)
{
	m_resultGroupItem->updateData(fn);
	renderView();
}

bool Graph2dScatteredWindowRootDataItem::axisNeeded(Graph2dScatteredWindowResultSetting::AxisSide as) const
{
	bool ret = false;
	ret = ret || m_resultGroupItem->axisNeeded(as);
	return ret;
}
