#include <guicore/project/projectdata.h>
#include "post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/xmlsupport.h>
#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowgraphicsview.h"

#include <vtkVertex.h>
#include <vtkTriangle.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkProperty.h>

#include <QIcon>
#include <QStandardItem>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QSignalMapper>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QTime>

#include <cgnslib.h>
#include <iriclib.h>

Post2dBirdEyeWindowZoneDataItem::Post2dBirdEyeWindowZoneDataItem(QString zoneName, int zoneNumber, GraphicsWindowDataItem* parent)
	: Post2dBirdEyeWindowDataItem(zoneName, QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_isDeletable = false;

	m_zoneName = zoneName;
	m_zoneNumber = zoneNumber;

	PostZoneDataContainer* cont = dataContainer();

	m_shapeDataItem = new Post2dBirdEyeWindowGridShapeDataItem(this);

	if (cont->scalarValueExists()){
		m_scalarGroupDataItem = new Post2dBirdEyeWindowNodeScalarGroupDataItem(this);
	}else{
		m_scalarGroupDataItem = nullptr;
	}

	m_childItems.append(m_shapeDataItem);
	if (cont->scalarValueExists()){
		m_childItems.append(m_scalarGroupDataItem);
	}
}

void Post2dBirdEyeWindowZoneDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode shapeNode = iRIC::getChildNode(node, "Shape");
	if (! shapeNode.isNull()){
		m_shapeDataItem->loadFromProjectMainFile(shapeNode);
	}
	QDomNode scalarGroupNode = iRIC::getChildNode(node, "ScalarGroup");
	if (! scalarGroupNode.isNull() && m_scalarGroupDataItem != nullptr){
		m_scalarGroupDataItem->loadFromProjectMainFile(scalarGroupNode);
	}
}

void Post2dBirdEyeWindowZoneDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_zoneName);
	writer.writeStartElement("Shape");
	m_shapeDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	if (m_scalarGroupDataItem != nullptr){
		writer.writeStartElement("ScalarGroup");
		m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void Post2dBirdEyeWindowZoneDataItem::addCustomMenuItems(QMenu* /*menu*/)
{

}

void Post2dBirdEyeWindowZoneDataItem::informSelection(VTKGraphicsView *v)
{
	m_shapeDataItem->informSelection(v);
}

void Post2dBirdEyeWindowZoneDataItem::informDeselection(VTKGraphicsView *v)
{
	m_shapeDataItem->informDeselection(v);
}

PostZoneDataContainer* Post2dBirdEyeWindowZoneDataItem::dataContainer()
{
	return postSolutionInfo()->zoneContainer2D(m_zoneName);
}

void Post2dBirdEyeWindowZoneDataItem::update()
{
	QTime time;
	time.start();
	m_shapeDataItem->update();
	qDebug("Grid shape: %d", time.elapsed());

	if (m_scalarGroupDataItem != nullptr){
		time.restart();
		m_scalarGroupDataItem->update();
		qDebug("Contour shape: %d", time.elapsed());
	}
}

void Post2dBirdEyeWindowZoneDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(m_childItems.count() + 1);
}

void Post2dBirdEyeWindowZoneDataItem::assignActionZValues(const ZDepthRange& range)
{
	int itemCount = m_childItems.count();
	int gapCount = itemCount - 1;
	float gapRate = .1; // the rate of gap width againt data width.

	double divWidth = range.width() / (itemCount + gapCount * gapRate);

	ZDepthRange r;
	double max, min;
	PostZoneDataContainer* cont = dataContainer();

	// the first is grid shape.
	max = range.max();
	min = max - divWidth;
	r = m_shapeDataItem->zDepthRange();
	r.setRange(min, max);
	m_shapeDataItem->setZDepthRange(r);

	// Contour
	if (cont->scalarValueExists()){
		max = min - divWidth * gapRate;
		min = max - divWidth;
		r = m_scalarGroupDataItem->zDepthRange();
		r.setRange(min, max);
		m_scalarGroupDataItem->setZDepthRange(r);
	}
}
