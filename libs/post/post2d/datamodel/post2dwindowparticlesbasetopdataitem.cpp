#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowParticlesBaseTopDataItem::Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	m_scalarGroupDataItem {nullptr},
	m_vectorGroupDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_showAttributeBrowserAction = new QAction(tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
}

Post2dWindowParticlesBaseTopDataItem::~Post2dWindowParticlesBaseTopDataItem()
{}

void Post2dWindowParticlesBaseTopDataItem::setup()
{
	bool vectorExist = false;
	vtkPolyData* partD = particleData();
	auto pd = partD->GetPointData();

	int num = pd->GetNumberOfArrays();
	for (int i = 0; i < num; i++) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attribute.
			vectorExist = true;
		}
	}
	m_scalarGroupDataItem = new Post2dWindowParticlesBaseScalarGroupDataItem(this);
	m_childItems.push_back(m_scalarGroupDataItem);

	if (vectorExist) {
		m_vectorGroupDataItem = new Post2dWindowParticlesBaseVectorGroupDataItem(this);
		m_childItems.push_back(m_vectorGroupDataItem);
	}
}


Post2dWindowParticlesBaseScalarGroupDataItem* Post2dWindowParticlesBaseTopDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post2dWindowParticlesBaseVectorGroupDataItem* Post2dWindowParticlesBaseTopDataItem::vectorGroupDataItem() const
{
	return m_vectorGroupDataItem;
}

QAction* Post2dWindowParticlesBaseTopDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void Post2dWindowParticlesBaseTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post2dWindowParticlesBaseTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	double center = (range.min() + range.max()) * 0.5;

	if (m_scalarGroupDataItem != nullptr) {
		auto r2 = range;
		r2.setMin(center);
		m_scalarGroupDataItem->assignActorZValues(r2);
	}

	if (m_vectorGroupDataItem != nullptr) {
		auto r2 = range;
		r2.setMax(center);
		m_vectorGroupDataItem->assignActorZValues(r2);
	}
}

void Post2dWindowParticlesBaseTopDataItem::update()
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->update();
	}
	if (m_vectorGroupDataItem != nullptr) {
		m_vectorGroupDataItem->update();
	}
}

void Post2dWindowParticlesBaseTopDataItem::showAttributeBrowser()
{
	zoneDataItem()->initParticleResultAttributeBrowser(particleData());
	auto w = dynamic_cast<Post2dWindow*> (mainWindow());
	w->propertyBrowser()->show();
}

void Post2dWindowParticlesBaseTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (m_scalarGroupDataItem != nullptr) {
		QDomNode scalarNode = iRIC::getChildNode(node, "ScalarGroup");
		if (! scalarNode.isNull()) {
			m_scalarGroupDataItem->loadFromProjectMainFile(scalarNode);
		}
	}
	if (m_vectorGroupDataItem != nullptr) {
		QDomNode vectorNode = iRIC::getChildNode(node, "VectorGroup");
		if (! vectorNode.isNull()) {
			m_vectorGroupDataItem->loadFromProjectMainFile(vectorNode);
		}
	}
}

void Post2dWindowParticlesBaseTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("ScalarGroup");
		m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (m_vectorGroupDataItem != nullptr) {
		writer.writeStartElement("VectorGroup");
		m_vectorGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post2dWindowParticlesBaseTopDataItem::propertyDialog(QWidget* parent)
{
	return m_scalarGroupDataItem->propertyDialog(parent);
}

void Post2dWindowParticlesBaseTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
}
