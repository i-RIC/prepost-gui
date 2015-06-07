#include "../post2dwindowgraphicsview.h"
#include "post2dwindowmeasureddatatopdataitem.h"
#include "post2dwindowrootdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/measureddata/measureddatafiledataitem.h>
#include <guicore/measureddata/measureddatapointgroupdataitem.h>
#include <guicore/measureddata/measureddatavectorgroupdataitem.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>
#include <pre/misc/preprocessorscalarbarlegendboxsettingdialog.h>

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

Post2dWindowMeasuredDataTopDataItem::Post2dWindowMeasuredDataTopDataItem(GraphicsWindowDataItem* parent)
	: Post2dWindowDataItem(tr("Measured Values"), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
{
	m_subFolder = "measureddata";

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);
	m_standardItem->setData(QVariant(tr("MEASUREDDATAS")), Qt::UserRole + 10);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();

	m_importAction = new QAction(QIcon(":/libs/guibase/images/iconImport.png"), tr("&Import..."), this);

	connect(m_importAction, SIGNAL(triggered()), iricMainWindow(), SLOT(importMeasuredData()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataAdded()), this, SLOT(addChildItem()));
	connect(projectData()->mainfile(), SIGNAL(measuredDataDeleted(int)), this, SLOT(deleteChildItem(int)));
	connect(dynamic_cast<Post2dWindowRootDataItem*>(this->parent()), SIGNAL(standardModelSetuped()), this, SLOT(setupChildItem()));
	connect(this, SIGNAL(selectMeasuredData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
}

Post2dWindowMeasuredDataTopDataItem::~Post2dWindowMeasuredDataTopDataItem()
{

}

void Post2dWindowMeasuredDataTopDataItem::setupChildItem()
{
	if (projectData()->mainfile()->measuredDatas().count() == 0) {return;}
	const QList<MeasuredData*>& measuredDatas = projectData()->mainfile()->measuredDatas();
	for (auto it = measuredDatas.begin(); it != measuredDatas.end(); ++it) {
		MeasuredDataFileDataItem* fItem = new MeasuredDataFileDataItem(*it, this);
		// there is no need to make the standard item top.
		m_childItems.push_back(fItem);
	}
	updateItemMap();
	updateZDepthRange();

	dataModel()->graphicsView()->ResetCameraClippingRange();
	setModified();
}

void Post2dWindowMeasuredDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{

}

void Post2dWindowMeasuredDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("MeasuredDataFile");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

const QList<MeasuredDataFileDataItem*> Post2dWindowMeasuredDataTopDataItem::fileDataItems() const
{
	QList<MeasuredDataFileDataItem*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		MeasuredDataFileDataItem* item = dynamic_cast<MeasuredDataFileDataItem*>(*it);
		ret.append(item);
	}
	return ret;
}

void Post2dWindowMeasuredDataTopDataItem::setupActors()
{

}

void Post2dWindowMeasuredDataTopDataItem::updateActorSettings()
{

}

void Post2dWindowMeasuredDataTopDataItem::addChildItem()
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

	dataModel()->graphicsView()->ResetCameraClippingRange();
	dataModel()->fit();
	emit selectMeasuredData(fItem->standardItem()->index());
}

void Post2dWindowMeasuredDataTopDataItem::deleteChildItem(int index)
{
	auto it = m_childItems.begin();
	delete *(it + index);
	dynamic_cast<Post2dWindowRootDataItem*>(parent())->updateItemMap();
	renderGraphicsView();
}

void Post2dWindowMeasuredDataTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(m_importAction);
}
