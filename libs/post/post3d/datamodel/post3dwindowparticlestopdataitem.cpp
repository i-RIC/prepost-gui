#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowparticlesscalargroupdataitem.h"
#include "post3dwindowparticlesvectorgroupdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowparticlestopdataitem_setsettingcommand.h"

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

Post3dWindowParticlesTopDataItem::Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem {tr("Particles"), QIcon(":/libs/guibase/images/iconPaper.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	bool scalarExist = false;
	bool vectorExist = false;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
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
		m_scalarGroupDataItem = new Post3dWindowParticlesScalarGroupDataItem(this);
		m_childItems.push_back(m_scalarGroupDataItem);
	} else {
		m_scalarGroupDataItem = nullptr;
		setupActors();
	}
	if (vectorExist) {
		m_vectorGroupDataItem = new Post3dWindowParticlesVectorGroupDataItem(this);
		m_childItems.push_back(m_vectorGroupDataItem);
	}	else {
		m_vectorGroupDataItem = nullptr;
	}

	updateActorSettings();
}

Post3dWindowParticlesTopDataItem::~Post3dWindowParticlesTopDataItem()
{
	if (m_actor == nullptr) {return;}
	renderer()->RemoveActor(m_actor);

//	delete m_scalarGroupDataItem;
//	delete m_vectorGroupDataItem;
}

Post3dWindowParticlesScalarGroupDataItem* Post3dWindowParticlesTopDataItem::scalarGroupDataItem() const
{
	return m_scalarGroupDataItem;
}

Post3dWindowParticlesVectorGroupDataItem* Post3dWindowParticlesTopDataItem::vectorGroupDataItem() const
{
	return m_vectorGroupDataItem;
}

QColor Post3dWindowParticlesTopDataItem::color() const
{
	return m_setting.color;
}

void Post3dWindowParticlesTopDataItem::setColor(const QColor& c)
{
	m_setting.color = c;
}

int Post3dWindowParticlesTopDataItem::size() const
{
	return m_setting.size;
}

void Post3dWindowParticlesTopDataItem::setSize(int s)
{
	m_setting.size = s;
}

void Post3dWindowParticlesTopDataItem::updateActorSettings()
{
	if (m_actor == nullptr) {return;}

	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}

	m_actor->GetProperty()->SetPointSize(m_setting.size);
	m_actor->GetProperty()->SetColor(m_setting.color);
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowParticlesTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post3dWindowParticlesTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post3dWindowParticlesTopDataItem::update()
{
	updateActorSettings();
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->update();
	}
	if (m_vectorGroupDataItem != nullptr) {
		m_vectorGroupDataItem->update();
	}
}

void Post3dWindowParticlesTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post3dWindowParticlesTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

void Post3dWindowParticlesTopDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);
}

QDialog* Post3dWindowParticlesTopDataItem::propertyDialog(QWidget* parent)
{
	if (m_scalarGroupDataItem != nullptr){
		return m_scalarGroupDataItem->propertyDialog(parent);
	}

	PostParticleBasicSettingDialog* dialog = new PostParticleBasicSettingDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

void Post3dWindowParticlesTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
		return;
	}
	PostParticleBasicSettingDialog* dialog = dynamic_cast<PostParticleBasicSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this);
}

void Post3dWindowParticlesTopDataItem::innerUpdateZScale(double scale)
{
	if (m_scalarGroupDataItem == nullptr) {
		m_actor->SetScale(1, 1, scale);
	}
}
