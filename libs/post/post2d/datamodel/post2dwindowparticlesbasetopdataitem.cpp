#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasetopdataitem_setsettingcommand.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>
#include <postbase/particle/postparticlebasicsettingdialog.h>

#include <QAction>
#include <QDomNode>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowParticlesBaseTopDataItem::Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_scalarGroupDataItem {nullptr},
	m_vectorGroupDataItem {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	m_showAttributeBrowserAction = new QAction(tr("Show Attribute Browser"), this);
	connect(m_showAttributeBrowserAction, SIGNAL(triggered()), this, SLOT(showAttributeBrowser()));
}

Post2dWindowParticlesBaseTopDataItem::~Post2dWindowParticlesBaseTopDataItem()
{
	if (m_actor == nullptr) {return;}
	renderer()->RemoveActor(m_actor);
}

void Post2dWindowParticlesBaseTopDataItem::setup()
{
	bool scalarExist = false;
	bool vectorExist = false;
	vtkPolyData* partD = particleData();
	vtkPointData* pd = partD->GetPointData();

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
		scalarExist = true;
	}
	if (scalarExist) {
		m_scalarGroupDataItem = new Post2dWindowParticlesBaseScalarGroupDataItem(this);
		m_childItems.push_back(m_scalarGroupDataItem);
	} else {
		setupActors();
	}
	if (vectorExist) {
		m_vectorGroupDataItem = new Post2dWindowParticlesBaseVectorGroupDataItem(this);
		m_childItems.push_back(m_vectorGroupDataItem);
	}
	updateActorSettings();
}


Post2dWindowParticlesBaseScalarGroupDataItem* Post2dWindowParticlesBaseTopDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post2dWindowParticlesBaseVectorGroupDataItem* Post2dWindowParticlesBaseTopDataItem::vectorGroupDataItem() const
{
	return m_vectorGroupDataItem;
}

QColor Post2dWindowParticlesBaseTopDataItem::color() const
{
	return m_setting.color;
}

void Post2dWindowParticlesBaseTopDataItem::setColor(const QColor& c)
{
	m_setting.color = c;
}

int Post2dWindowParticlesBaseTopDataItem::size() const
{
	return m_setting.size;
}

void Post2dWindowParticlesBaseTopDataItem::setSize(int s)
{
	m_setting.size = s;
}

QAction* Post2dWindowParticlesBaseTopDataItem::showAttributeBrowserAction() const
{
	return m_showAttributeBrowserAction;
}

void Post2dWindowParticlesBaseTopDataItem::updateActorSettings()
{
	if (m_actor == nullptr) {return;}

	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto pData = particleData();
	if (pData == nullptr) {return;}

	m_actor->GetProperty()->SetPointSize(m_setting.size);
	m_actor->GetProperty()->SetColor(m_setting.color);
	m_mapper->SetInputData(pData);

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesBaseTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post2dWindowParticlesBaseTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_actor != nullptr){
		m_actor->SetPosition(0, 0, range.min());
	}
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
	updateActorSettings();
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
	m_setting.load(node);

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
	m_setting.save(writer);

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

void Post2dWindowParticlesBaseTopDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);
}

QDialog* Post2dWindowParticlesBaseTopDataItem::propertyDialog(QWidget* parent)
{
	if (m_scalarGroupDataItem != nullptr){
		return m_scalarGroupDataItem->propertyDialog(parent);
	}

	PostParticleBasicSettingDialog* dialog = new PostParticleBasicSettingDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

void Post2dWindowParticlesBaseTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
		return;
	}
	PostParticleBasicSettingDialog* dialog = dynamic_cast<PostParticleBasicSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this);
}
