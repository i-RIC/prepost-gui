#include "rootdataitem.h"
#include "../base/dataitemview.h"
#include "../baseline/baseline.h"
#include "../crosssections/crosssections.h"
#include "../elevationpoints/elevationpoints.h"
#include "../watersurfaceelevationpoints/watersurfaceelevationpoints.h"
#include "../project/project.h"

#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QPointF>
#include <QStandardItem>
#include <QXmlStreamWriter>

RootDataItem::RootDataItem(Project* project) :
	DataItem {},
	m_project {project}
{}

RootDataItem::~RootDataItem()
{}

Project* RootDataItem::project() const
{
	return m_project;
}

QStandardItem* RootDataItem::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem("Root Item");
	setupStandardItem(item);
	return item;
}

DataItemView* RootDataItem::buildPreProcessorDataItemView(Model *model)
{
	return new DataItemView(model, this);
}

void RootDataItem::doLoadFromMainFile(const QDomElement& node)
{
	double offsetx, offsety;
	offsetx = iRIC::getDoubleAttribute(node, "offsetX");
	offsety = iRIC::getDoubleAttribute(node, "offsetY");
	m_project->setOffset(QPointF(offsetx, offsety));

	m_project->setRivFileName(node.attribute("rivFileName", ""));
	m_project->setCsvFileName(node.attribute("csvFileName", ""));

	QDomElement epElem = iRIC::getChildNode(node, "ElevationPoints").toElement();
	if (! epElem.isNull()) {
		project()->elevationPoints().loadFromMainFile(epElem);
	}

	QDomElement wsepElem = iRIC::getChildNode(node, "WaterSurfaceElevationPoints").toElement();
	if (! wsepElem.isNull()) {
		project()->waterSurfaceElevationPoints().loadFromMainFile(wsepElem);
	}

	QDomElement csElem = iRIC::getChildNode(node, "CrossSections").toElement();
	if (! csElem.isNull()) {
		project()->crossSections().loadFromMainFile(csElem);
	}

	QDomElement blElem = iRIC::getChildNode(node, "BaseLine").toElement();
	if (! blElem.isNull()) {
		project()->baseLine().loadFromMainFile(blElem);
	}
}

void RootDataItem::doSaveToMainFile(QXmlStreamWriter* writer) const
{
	auto offset = m_project->offset();
	iRIC::setDoubleAttribute(*writer, "offsetX", offset.x());
	iRIC::setDoubleAttribute(*writer, "offsetY", offset.y());

	writer->writeAttribute("rivFileName", m_project->rivFileName());
	writer->writeAttribute("csvFileName", m_project->csvFileName());

	writer->writeStartElement("ElevationPoints");
	project()->elevationPoints().saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("WaterSurfaceElevationPoints");
	project()->waterSurfaceElevationPoints().saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("CrossSections");
	project()->crossSections().saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("BaseLine");
	project()->baseLine().saveToMainFile(writer);
	writer->writeEndElement();
}
