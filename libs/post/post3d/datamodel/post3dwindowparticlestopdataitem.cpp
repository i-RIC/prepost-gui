#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <postbase/postparticlebasicpropertydialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

#include <vtkRenderer.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <QSettings>

Post3dWindowParticlesTopDataItem::Post3dWindowParticlesTopDataItem(Post3dWindowDataItem* p)
	: Post3dWindowDataItem(tr("Particles"), QIcon(":/libs/guibase/images/iconPaper.png"), p)
{
	QSettings setting;

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();
	m_color = setting.value("graphics/particlecolor", QColor(Qt::black)).value<QColor>();
	m_size = setting.value("graphics/particlesize", 3).toInt();

	setupActors();
	updateActorSettings();
}

Post3dWindowParticlesTopDataItem::~Post3dWindowParticlesTopDataItem()
{
	renderer()->RemoveActor(m_actor);
}

void Post3dWindowParticlesTopDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr){return;}

	m_actor->GetProperty()->SetPointSize(m_size);
	m_actor->GetProperty()->SetColor(m_color.redF(), m_color.greenF(), m_color.blueF());
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowParticlesTopDataItem::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void Post3dWindowParticlesTopDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post3dWindowParticlesTopDataItem::update()
{
	updateActorSettings();
}

void Post3dWindowParticlesTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_color = iRIC::getColorAttribute(node, "color");
	m_size = iRIC::getIntAttribute(node, "size", 3);
}

void Post3dWindowParticlesTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setColorAttribute(writer, "color", m_color);
	iRIC::setIntAttribute(writer, "size", m_size);
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
	PostParticleBasicPropertyDialog* dialog = new PostParticleBasicPropertyDialog(parent);
	dialog->setColor(m_color);
	dialog->setSize(m_size);

	return dialog;
}

class Post3dWindowParticlesTopSetProperty : public QUndoCommand
{
public:
	Post3dWindowParticlesTopSetProperty(const QColor& color, int size, Post3dWindowParticlesTopDataItem* item)
	{
		m_newColor = color;
		m_newSize = size;

		m_oldColor = item->m_color;
		m_oldSize = item->m_size;

		m_item = item;
	}

	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->m_color = m_oldColor;
		m_item->m_size = m_oldSize;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->m_color = m_newColor;
		m_item->m_size = m_newSize;
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}

private:
	QColor m_oldColor;
	int m_oldSize;

	QColor m_newColor;
	int m_newSize;

	Post3dWindowParticlesTopDataItem* m_item;
};

void Post3dWindowParticlesTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleBasicPropertyDialog* dialog = dynamic_cast<PostParticleBasicPropertyDialog*>(propDialog);
	iRICUndoStack::instance().push(new Post3dWindowParticlesTopSetProperty(dialog->color(), dialog->size(), this));
}

void Post3dWindowParticlesTopDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}
