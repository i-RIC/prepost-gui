#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "private/measureddatafiledataitem_impl.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/project/measured/measureddatacsvexporter.h>
#include <misc/errormessage.h>
#include <misc/lastiodirectory.h>
#include <misc/xmlsupport.h>
#include <pre/preprocessordatamodel.h>

#include <QAction>
#include <QDir>
#include <QDomElement>
#include <QDomNode>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItem>
#include <QStatusBar>
#include <QVector2D>
#include <QXmlStreamWriter>

MeasuredDataFileDataItem::Impl::Impl(MeasuredData* md) :
	m_measuredData {md},
	m_exportAction {nullptr}
{}

// public interfaces

MeasuredDataFileDataItem::MeasuredDataFileDataItem(MeasuredData* md, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("File"), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	impl {new Impl{md}}
{
	setSubPath("file");
	setupStandardItem(Checked, NotReorderable, Deletable, md->name());

	impl->m_pointGroupDataItem = new MeasuredDataPointGroupDataItem(this);
	impl->m_vectorGroupDataItem = new MeasuredDataVectorGroupDataItem(this);

	m_childItems.push_back(impl->m_pointGroupDataItem);
	m_childItems.push_back(impl->m_vectorGroupDataItem);

	impl->m_exportAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"), MeasuredDataFileDataItem::tr("&Export..."), this);
	connect(impl->m_exportAction, SIGNAL(triggered()), this, SLOT(exportToFile()));
}

MeasuredDataFileDataItem::~MeasuredDataFileDataItem()
{
	delete impl;
}

void MeasuredDataFileDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode pdNode = iRIC::getChildNode(node, "PointData");
	if (! pdNode.isNull()) {impl->m_pointGroupDataItem->loadFromProjectMainFile(pdNode);}
	QDomNode vdNode = iRIC::getChildNode(node, "VectorData");
	if (! vdNode.isNull()) {impl->m_vectorGroupDataItem->loadFromProjectMainFile(vdNode);}
}

void MeasuredDataFileDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("index", QString::number(impl->m_measuredData->index()));

	writer.writeStartElement("PointData");
	impl->m_pointGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();

	writer.writeStartElement("VectorData");
	impl->m_vectorGroupDataItem->saveToProjectMainFile(writer);
	writer.writeEndElement();
}

void MeasuredDataFileDataItem::exportToFile()
{
	QString dir = LastIODirectory::get();
	QString filter(tr("CSV file (*.csv)"));
	QString fname = QFileDialog::getSaveFileName(iricMainWindow(), tr("Export Measured Data"), dir, filter);
	if (fname == "") { return; }

	try {
		MeasuredDataCsvExporter exporter;
		QVector2D of = offset();
		QPointF local_offset = QPointF(of.x(), of.y());
		exporter.exportData(fname, local_offset, *(impl->m_measuredData));

		iricMainWindow()->statusBar()->showMessage(tr("Measured Data successfully exported to %1.").arg(QDir::toNativeSeparators(fname)), iRICMainWindowInterface::STATUSBAR_DISPLAYTIME);
	} catch (ErrorMessage& message) {
		QMessageBox::critical(iricMainWindow(), tr("Error"), message);
	}
}


MeasuredData* MeasuredDataFileDataItem::measuredData() const
{
	return impl->m_measuredData;
}

MeasuredDataPointGroupDataItem* MeasuredDataFileDataItem::pointGroupDataItem() const
{
	return impl->m_pointGroupDataItem;
}

MeasuredDataVectorGroupDataItem* MeasuredDataFileDataItem::vectorGroupDataItem() const
{
	return impl->m_vectorGroupDataItem;
}

void MeasuredDataFileDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_exportAction);
}

void MeasuredDataFileDataItem::doApplyOffset(double x, double y)
{
	if (PreProcessorDataModel* p = dynamic_cast<PreProcessorDataModel*>(dataModel())) {
		// since m_measuredData is shared, only apply offset from preprocessor
		impl->m_measuredData->applyOffset(x, y);
	}
}
