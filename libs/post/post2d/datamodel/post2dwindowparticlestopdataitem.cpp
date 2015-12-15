#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowparticlesscalargroupdataitem.h"
#include "post2dwindowparticlesvectorgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>
#include <postbase/postparticlebasicpropertydialog.h>

#include <QSettings>

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
	}
	else {
		m_vectorGroupDataItem = nullptr;
	}

	updateActorSettings();
}

Post2dWindowParticlesTopDataItem::~Post2dWindowParticlesTopDataItem()
{
	if (m_actor == nullptr) {return;}
	renderer()->RemoveActor(m_actor);
	if (m_scalarGroupDataItem != nullptr) {
		delete m_scalarGroupDataItem;
	}
	if (m_vectorGroupDataItem != nullptr) {
		delete m_vectorGroupDataItem;
	}
}

int Post2dWindowParticlesTopDataItem::size() const
{
	return m_setting.size;
}

void Post2dWindowParticlesTopDataItem::setSize(int size)
{
	m_setting.size = size;
}

QColor Post2dWindowParticlesTopDataItem::color() const
{
	return m_setting.color;
}

void Post2dWindowParticlesTopDataItem::setColor(const QColor& color)
{
	m_setting.color = color;
}

void Post2dWindowParticlesTopDataItem::updateActorSettings()
{
	if (m_actor == nullptr) {return;}

	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == 0 || cont->particleData() == 0) {return;}

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

	PostParticleBasicPropertyDialog* dialog = new PostParticleBasicPropertyDialog(parent);
	dialog->setSetting(m_setting);

	return dialog;
}

class Post2dWindowParticlesTopDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const PostParticleBasicPropertyDialog::Setting s, Post2dWindowParticlesTopDataItem* item) :
		QUndoCommand {Post2dWindowParticlesTopDataItem::tr("Edit Particle Display Setting")},
		m_newSetting {s},
		m_oldSetting {item->m_setting},
		m_item {item}
	{}

	void redo() {
		m_item->m_setting = m_newSetting;
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->m_setting = m_oldSetting;
		m_item->updateActorSettings();
	}

private:
	PostParticleBasicPropertyDialog::Setting m_newSetting;
	PostParticleBasicPropertyDialog::Setting m_oldSetting;

	Post2dWindowParticlesTopDataItem* m_item;
};

void Post2dWindowParticlesTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	if (m_scalarGroupDataItem != nullptr) {
		m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
		return;
	}
	PostParticleBasicPropertyDialog* dialog = dynamic_cast<PostParticleBasicPropertyDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, false);
}
