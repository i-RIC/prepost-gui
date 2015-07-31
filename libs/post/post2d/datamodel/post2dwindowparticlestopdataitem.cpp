#include "post2dwindowparticlestopdataitem.h"
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

Post2dWindowParticlesTopDataItem::Post2dWindowParticlesTopDataItem(Post2dWindowDataItem* p)
	: Post2dWindowDataItem(tr("Particles"), QIcon(":/libs/guibase/images/iconPaper.png"), p)
{
	QSettings setting;

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
	updateActorSettings();
}

Post2dWindowParticlesTopDataItem::~Post2dWindowParticlesTopDataItem()
{
	renderer()->RemoveActor(m_actor);
}

void Post2dWindowParticlesTopDataItem::updateActorSettings()
{
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
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowParticlesTopDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowParticlesTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
}

void Post2dWindowParticlesTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
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
	PostParticleBasicPropertyDialog* dialog = dynamic_cast<PostParticleBasicPropertyDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, false);
}
