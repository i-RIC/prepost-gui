#include "watersurfaceelevationpoints.h"
#include "private/watersurfaceelevationpoints_impl.h"
#include "../project/project.h"
#include "../base/dataitemview.h"

#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QStandardItem>
#include <QXmlStreamWriter>

WaterSurfaceElevationPoints::Impl::Impl(WaterSurfaceElevationPoints *parent) :
	m_arbitraryHWM {parent},
	m_benchmark {parent},
	m_referenceMark {parent},
	m_hub {parent},
	m_leftBankHWM {parent},
	m_rightBankHWM {parent},
	m_streamGage {parent}
{}

WaterSurfaceElevationPoints::WaterSurfaceElevationPoints(DataItem *parent) :
	DataItem {parent},
	impl {new Impl {this}}
{}

WaterSurfaceElevationPoints::~WaterSurfaceElevationPoints()
{
	delete impl;
}

const LeftBankHWM& WaterSurfaceElevationPoints::leftBankHWM() const
{
	return impl->m_leftBankHWM;
}

LeftBankHWM& WaterSurfaceElevationPoints::leftBankHWM()
{
	return impl->m_leftBankHWM;
}

const RightBankHWM& WaterSurfaceElevationPoints::rightBankHWM() const
{
	return impl->m_rightBankHWM;
}

RightBankHWM& WaterSurfaceElevationPoints::rightBankHWM()
{
	return impl->m_rightBankHWM;
}

const ArbitraryHWM& WaterSurfaceElevationPoints::arbitraryHWM() const
{
	return impl->m_arbitraryHWM;
}

ArbitraryHWM& WaterSurfaceElevationPoints::arbitraryHWM()
{
	return impl->m_arbitraryHWM;
}

const Benchmark& WaterSurfaceElevationPoints::benchmark() const
{
	return impl->m_benchmark;
}

Benchmark& WaterSurfaceElevationPoints::benchmark()
{
	return impl->m_benchmark;
}

const Hub& WaterSurfaceElevationPoints::hub() const
{
	return impl->m_hub;
}

Hub& WaterSurfaceElevationPoints::hub()
{
	return impl->m_hub;
}

const ReferenceMark& WaterSurfaceElevationPoints::referenceMark() const
{
	return impl->m_referenceMark;
}

ReferenceMark& WaterSurfaceElevationPoints::referenceMark()
{
	return impl->m_referenceMark;
}

const StreamGage& WaterSurfaceElevationPoints::streamGage() const
{
	return impl->m_streamGage;
}

StreamGage& WaterSurfaceElevationPoints::streamGage()
{
	return impl->m_streamGage;
}

QStandardItem* WaterSurfaceElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconFolder.png"), tr("Water Elevation Points"));
	setupStandardItem(item);
	return item;
}

DataItemView* WaterSurfaceElevationPoints::buildPreProcessorDataItemView(Model* model)
{
	return new DataItemView(model, this);
}

void WaterSurfaceElevationPoints::doLoadFromMainFile(const QDomElement& node)
{
	QDomElement aElem = iRIC::getChildNode(node, "ArbitraryHWM").toElement();
	if (! aElem.isNull()) {
		impl->m_arbitraryHWM.loadFromMainFile(aElem);
	}

	QDomElement bmElem = iRIC::getChildNode(node, "Benchmark").toElement();
	if (! bmElem.isNull()) {
		impl->m_benchmark.loadFromMainFile(bmElem);
	}

	QDomElement rmElem = iRIC::getChildNode(node, "referenceMark").toElement();
	if (! rmElem.isNull()) {
		impl->m_referenceMark.loadFromMainFile(rmElem);
	}

	QDomElement hElem = iRIC::getChildNode(node, "hub").toElement();
	if (! hElem.isNull()) {
		impl->m_hub.loadFromMainFile(hElem);
	}

	QDomElement lbElem = iRIC::getChildNode(node, "leftBankHWM").toElement();
	if (! lbElem.isNull()) {
		impl->m_leftBankHWM.loadFromMainFile(lbElem);
	}

	QDomElement rbElem = iRIC::getChildNode(node, "rightBankHWM").toElement();
	if (! rbElem.isNull()) {
		impl->m_rightBankHWM.loadFromMainFile(rbElem);
	}

	QDomElement sgElem = iRIC::getChildNode(node, "streamGage").toElement();
	if (! sgElem.isNull()) {
		impl->m_streamGage.loadFromMainFile(sgElem);
	}
}

void WaterSurfaceElevationPoints::doSaveToMainFile(QXmlStreamWriter* writer) const
{
	writer->writeStartElement("ArbitraryHWM");
	impl->m_arbitraryHWM.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("Benchmark");
	impl->m_benchmark.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("referenceMark");
	impl->m_referenceMark.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("hub");
	impl->m_hub.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("leftBankHWM");
	impl->m_leftBankHWM.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("rightBankHWM");
	impl->m_rightBankHWM.saveToMainFile(writer);
	writer->writeEndElement();

	writer->writeStartElement("streamGage");
	impl->m_streamGage.saveToMainFile(writer);
	writer->writeEndElement();
}
