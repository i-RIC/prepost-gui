#include "../graph2dhybridwindow.h"
#include "../graph2dhybridwindowdatamodel.h"
#include "../graph2dhybridwindowdatamodel.h"
#include "../graph2dhybridwindowobjectbrowser.h"
#include "../graph2dhybridwindowview.h"
#include "graph2dhybridwindowimportdatagroupdataitem.h"
#include "graph2dhybridwindowresultcopygroupdataitem.h"
#include "graph2dhybridwindowresultgroupdataitem.h"
#include "graph2dhybridwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include <post/graph2d/datamodel/graph2dwindowmarkergroupdataitem.h>

#include <QStandardItemModel>
#include <QXmlStreamWriter>

#include <vtkRenderWindow.h>

Graph2dHybridWindowRootDataItem::Graph2dHybridWindowRootDataItem(Graph2dHybridWindow* window, ProjectDataItem* parent)
	: Graph2dWindowRootDataItem(window, parent)
{
	m_resultGroupItem = new Graph2dHybridWindowResultGroupDataItem(this);
	m_childItems.append(m_resultGroupItem);

	m_resultCopyGroupItem = new Graph2dHybridWindowResultCopyGroupDataItem(this);
	m_childItems.append(m_resultCopyGroupItem);

	m_importDataGroupItem = new Graph2dHybridWindowImportDataGroupDataItem(this);
	m_childItems.append(m_importDataGroupItem);

	m_markerGroupItem = new Graph2dWindowMarkerGroupDataItem(this);
	m_markerGroupItem->setXLabel(tr("x"));
	m_childItems.append(m_markerGroupItem);
	updateItemMap();
}

Graph2dHybridWindowRootDataItem::~Graph2dHybridWindowRootDataItem()
{
	delete m_resultGroupItem;
	delete m_resultCopyGroupItem;
	delete m_importDataGroupItem;
	delete m_markerGroupItem;
}

void Graph2dHybridWindowRootDataItem::setupStandardModel(QStandardItemModel* model)
{
	model->clear();
	model->appendRow(m_resultGroupItem->standardItem());
	model->appendRow(m_markerGroupItem->standardItem());
	emit standardModelSetuped();
}

void Graph2dHybridWindowRootDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode resultNode = iRIC::getChildNode(node, "CalculationResult");
	if (! resultNode.isNull()) {
		m_resultGroupItem->loadFromProjectMainFile(resultNode);
	}
	QDomNode resultCopyNode = iRIC::getChildNode(node, "CalculationResultCopy");
	if (! resultCopyNode.isNull()) {
		m_resultCopyGroupItem->loadFromProjectMainFile(resultCopyNode);
	}
	QDomNode exteralNode = iRIC::getChildNode(node, "ExternalData");
	if (! exteralNode.isNull()) {
		m_importDataGroupItem->loadFromProjectMainFile(exteralNode);
	}
	QDomNode markerNode = iRIC::getChildNode(node, "Markers");
	if (! markerNode.isNull()) {
		m_markerGroupItem->loadFromProjectMainFile(markerNode);
	}
	updateItemMap();
}
void Graph2dHybridWindowRootDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("CalculationResult");
	m_resultGroupItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("CalculationResultCopy");
	m_resultCopyGroupItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("ExternalData");
	m_importDataGroupItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("Markers");
	m_markerGroupItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void Graph2dHybridWindowRootDataItem::updateData(int fn)
{
	m_resultGroupItem->updateData(fn);
	m_resultCopyGroupItem->updateData();
	m_importDataGroupItem->updateData();
	renderView();
}

/*
void Graph2dHybridWindowRootDataItem::updateTime()
{
//	m_resultGroupItem->updateTime();
	renderView();
}
*/

bool Graph2dHybridWindowRootDataItem::axisNeeded(Graph2dHybridWindowResultSetting::AxisSide as) const
{
	bool ret = false;
	ret = ret || m_resultGroupItem->axisNeeded(as);
	ret = ret || m_resultCopyGroupItem->axisNeeded(as);
	ret = ret || m_importDataGroupItem->axisNeeded(as);
	return ret;
}
