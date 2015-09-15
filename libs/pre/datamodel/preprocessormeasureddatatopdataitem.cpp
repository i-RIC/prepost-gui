#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
#include "../preprocessorgraphicsview.h"
#include "preprocessormeasureddatatopdataitem.h"
#include "preprocessorrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/measureddata/measureddatafiledataitem.h>
#include <guicore/measureddata/measureddatapointgroupdataitem.h>
#include <guicore/measureddata/measureddatavectorgroupdataitem.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/project/measureddata.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

#include <QAction>
#include <QDomNode>
#include <QIcon>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

PreProcessorMeasuredDataTopDataItem::PreProcessorMeasuredDataTopDataItem(GraphicsWindowDataItem* parent) :
	PreProcessorDataItem {tr("Measured Values"), QIcon(":/libs/guibase/images/iconFolder.png"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath("measureddata");

	setIsCommandExecuting(true);
	m_standardItem->setData(QVariant("MEASUREDDATAS"), Qt::UserRole + 10);
	setIsCommandExecuting(false);

	setupActors();

	m_importAction = new QAction(QIcon(":/libs/guibase/images/iconImport.png"), PreProcessorMeasuredDataTopDataItem::tr("&Import..."), this);

	connect(m_importAction, SIGNAL(triggered()), iricMainWindow(), SLOT(importMeasuredData()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataAdded()), this, SLOT(addChildItem()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(this, SIGNAL(selectMeasuredData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
}

PreProcessorMeasuredDataTopDataItem::~PreProcessorMeasuredDataTopDataItem()
{

}

void PreProcessorMeasuredDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	const QList<MeasuredData*>& mdlist = projectData()->mainfile()->measuredDatas();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		int index = child.attribute("index").toInt();
		for (int j = 0; j < mdlist.count(); ++j) {
			MeasuredData* md = mdlist.at(j);
			if (md->index() == index) {
				MeasuredDataFileDataItem* fitem = new MeasuredDataFileDataItem(md, this);
				fitem->loadFromProjectMainFile(child);
				m_childItems.append(fitem);
			}
		}
	}
}

void PreProcessorMeasuredDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("MeasuredDataFile");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

const QList<MeasuredDataFileDataItem*> PreProcessorMeasuredDataTopDataItem::fileDataItems() const
{
	QList<MeasuredDataFileDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		MeasuredDataFileDataItem* item = dynamic_cast<MeasuredDataFileDataItem*>(*it);
		ret.append(item);
	}
	return ret;
}

void PreProcessorMeasuredDataTopDataItem::setupActors()
{

}

void PreProcessorMeasuredDataTopDataItem::updateActorSettings()
{

}

void PreProcessorMeasuredDataTopDataItem::addChildItem()
{
	MeasuredData* md = projectData()->mainfile()->measuredDatas().last();
	MeasuredDataFileDataItem* fItem = new MeasuredDataFileDataItem(md, this);
	m_childItems.append(fItem);

	updateItemMap();
	updateZDepthRange();

	ObjectBrowserView* oview = dataModel()->objectBrowserView();
	oview->select(fItem->standardItem()->index());
	oview->expand(fItem->standardItem()->index());
	oview->expand(fItem->pointGroupDataItem()->standardItem()->index());
	oview->expand(fItem->vectorGroupDataItem()->standardItem()->index());
	dataModel()->graphicsView()->cameraFit();
	emit selectMeasuredData(fItem->standardItem()->index());
}

void PreProcessorMeasuredDataTopDataItem::deleteChildItem(int index)
{
	auto it = m_childItems.begin();
	delete *(it + index);
	dynamic_cast<PreProcessorRootDataItem*>(parent())->updateItemMap();
	renderGraphicsView();
}

void PreProcessorMeasuredDataTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_importAction);
}
