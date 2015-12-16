#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowparticlesscalargroupdataitem.h"
#include "post2dwindowparticlesvectorgroupdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlestopdataitem_setsettingcommand.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>
#include <postbase/particle/postparticlebasicsettingdialog.h>

#include <QDomNode>
#include <QSettings>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

Post2dWindowParticlesTopDataItem::Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Particles"), QIcon(":/libs/guibase/images/iconPaper.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	bool scalarExist = false;
	bool vectorExist = false;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	vtkPolyData* partD = cont->particleData();
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
		m_scalarGroupDataItem = new Post2dWindowParticlesScalarGroupDataItem(this);
		m_childItems.append(m_scalarGroupDataItem);
	} else {
		m_scalarGroupDataItem = nullptr;
		setupActors();
	}
	if (vectorExist) {
		m_vectorGroupDataItem = new Post2dWindowParticlesVectorGroupDataItem(this);
		m_childItems.append(m_vectorGroupDataItem);
	} else {
		m_vectorGroupDataItem = nullptr;
	}

	updateActorSettings();
}

Post2dWindowParticlesTopDataItem::~Post2dWindowParticlesTopDataItem()
{
	if (m_actor == nullptr) {return;}
	renderer()->RemoveActor(m_actor);

//	delete m_scalarGroupDataItem;
//	delete m_vectorGroupDataItem;
}

Post2dWindowParticlesScalarGroupDataItem* Post2dWindowParticlesTopDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post2dWindowParticlesVectorGroupDataItem* Post2dWindowParticlesTopDataItem::vectorGroupDataItem() const
{
	return m_vectorGroupDataItem;
}

QColor Post2dWindowParticlesTopDataItem::color() const
{
	return m_setting.color;
}

void Post2dWindowParticlesTopDataItem::setColor(const QColor& c)
{
	m_setting.color = c;
}

int Post2dWindowParticlesTopDataItem::size() const
{
	return m_setting.size;
}

void Post2dWindowParticlesTopDataItem::setSize(int s)
{
	m_setting.size = s;
}

void Post2dWindowParticlesTopDataItem::updateActorSettings()
{
	if (m_actor == nullptr) {return;}

	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}

	m_actor->GetProperty()->SetPointSize(m_setting.size);
	m_actor->GetProperty()->SetColor(m_setting.color);
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post2dWindowParticlesTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_actor == nullptr) {return;}
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowParticlesTopDataItem::update()
{
	updateActorSettings();
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->update();
	}
	if (m_vectorGroupDataItem != nullptr) {
		m_vectorGroupDataItem->update();
	}
}

void Post2dWindowParticlesTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post2dWindowParticlesTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

void Post2dWindowParticlesTopDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);
}

QDialog* Post2dWindowParticlesTopDataItem::propertyDialog(QWidget* parent)
{
	if (m_scalarGroupDataItem != nullptr){
		return m_scalarGroupDataItem->propertyDialog(parent);
	}

	PostParticleBasicSettingDialog* dialog = new PostParticleBasicSettingDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

void Post2dWindowParticlesTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
		return;
	}
	PostParticleBasicSettingDialog* dialog = dynamic_cast<PostParticleBasicSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this);
}
