#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post3dWindowParticlesBaseTopDataItem::Post3dWindowParticlesBaseTopDataItem(const QString& caption, Post3dWindowDataItem* p) :
	Post3dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_scalarGroupDataItem {nullptr},
	m_vectorGroupDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

Post3dWindowParticlesBaseTopDataItem::~Post3dWindowParticlesBaseTopDataItem()
{}

void Post3dWindowParticlesBaseTopDataItem::setup()
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
	m_scalarGroupDataItem = new Post3dWindowParticlesBaseScalarGroupDataItem(this);
	m_childItems.push_back(m_scalarGroupDataItem);

	if (vectorExist) {
		m_vectorGroupDataItem = new Post3dWindowParticlesBaseVectorGroupDataItem(this);
		m_childItems.push_back(m_vectorGroupDataItem);
	}
}

Post3dWindowParticlesBaseScalarGroupDataItem* Post3dWindowParticlesBaseTopDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post3dWindowParticlesBaseVectorGroupDataItem* Post3dWindowParticlesBaseTopDataItem::vectorGroupDataItem() const
{
	return m_vectorGroupDataItem;
}

void Post3dWindowParticlesBaseTopDataItem::update()
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->update();
	}
	if (m_vectorGroupDataItem != nullptr) {
		m_vectorGroupDataItem->update();
	}
}

void Post3dWindowParticlesBaseTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post3dWindowParticlesBaseTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

QDialog* Post3dWindowParticlesBaseTopDataItem::propertyDialog(QWidget* parent)
{
	return m_scalarGroupDataItem->propertyDialog(parent);
}

void Post3dWindowParticlesBaseTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
}
