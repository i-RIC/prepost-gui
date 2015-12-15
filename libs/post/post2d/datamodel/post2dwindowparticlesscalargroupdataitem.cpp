#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesscalargroupdataitem.h"
#include "post2dwindowparticlesscalardataitem.h"
#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guibase/graphicsmisc.h>
#include <postbase/postparticlescalarpropertydialog.h>

#include <QDomElement>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActor.h>

Post2dWindowParticlesScalarGroupDataItem::Post2dWindowParticlesScalarGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	vtkPointData* pd = cont->particleData()->GetPointData();
	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attribute.
			continue;
		}
		std::string name = pd->GetArray(i)->GetName();
		Post2dWindowParticlesScalarDataItem* item = new Post2dWindowParticlesScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
		m_scalarbarTitleMap.insert(name, name.c_str());
	}
	m_titleTextSetting.setPrefix("titleText");
	m_labelTextSetting.setPrefix("labelText");
}

Post2dWindowParticlesScalarGroupDataItem::~Post2dWindowParticlesScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_actor);
	m_scalarBarWidget->SetInteractor(0);
}


std::string Post2dWindowParticlesScalarGroupDataItem::target() const
{
	return m_target;
}

void Post2dWindowParticlesScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void Post2dWindowParticlesScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowParticlesScalarGroupDataItem::assignActionZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowParticlesScalarGroupDataItem::update()
{
	updateActorSettings();
}

QDialog* Post2dWindowParticlesScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post2dWindowParticlesTopDataItem* tItem =
			dynamic_cast<Post2dWindowParticlesTopDataItem*> (parent());

	m_scalarbarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	PostParticleScalarPropertyDialog* dialog = new PostParticleScalarPropertyDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	dialog->setZoneData(cont);
	dialog->setTarget(target());
	dialog->setScalarBarTitleMap(m_scalarbarTitleMap);
	dialog->setScalarBarSetting(m_scalarbarSetting);
	dialog->setTitleTextSetting(m_titleTextSetting);
	dialog->setLabelTextSetting(m_labelTextSetting);
	dialog->setParticleSize(tItem->size());
	dialog->setCustomColor(tItem->color());

	return dialog;
}

class Post2dWindowParticlesScalarSetProperty : public QUndoCommand
{
public:
	Post2dWindowParticlesScalarSetProperty(const std::string& target, const LookupTableContainer& ltc, int size, const QColor& color, QString colorbarTitle, ScalarBarSetting scalarBarSetting, const vtkTextPropertySettingContainer& titleCont, const vtkTextPropertySettingContainer& labelCont, Post2dWindowParticlesScalarGroupDataItem* item) :
		QUndoCommand(QObject::tr("Update Particles Scalar Setting"))
	{
		m_newTarget = target;
		m_newLookupTable = ltc;
		m_newScalarBarTitle = colorbarTitle;
		m_newSize = size;
		m_newCustomColor = color;
		m_newScalarBarSetting = scalarBarSetting;
		m_newTitleTextSetting = titleCont;
		m_newLabelTextSetting = labelCont;

		m_oldTarget = item->m_target;
		if (m_newTarget != ""){
			Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(item->parent()->parent()->parent());
			LookupTableContainer* lut = gtItem->particleLookupTable(m_newTarget);
			m_oldLookupTable = *lut;

			m_oldScalarBarTitle = item->m_scalarbarTitleMap.value(target);
		}

		Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(item->parent());
		m_oldSize = tItem->size();
		m_oldCustomColor = tItem->color();
		m_oldScalarBarSetting = item->m_scalarbarSetting;
		m_oldTitleTextSetting = item->m_titleTextSetting;
		m_oldLabelTextSetting = item->m_labelTextSetting;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(m_item->parent());
		tItem->setSize(m_oldSize);
		tItem->setColor(m_oldCustomColor);

		m_item->setTarget(m_oldTarget);
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
		if (m_newTarget != ""){
			LookupTableContainer* lut = gtItem->particleLookupTable(m_newTarget);
			*lut = m_oldLookupTable;
			lut->update();
			m_item->m_scalarbarTitleMap[m_newTarget] = m_oldScalarBarTitle;
		}
		m_item->m_scalarbarSetting = m_oldScalarBarSetting;
		m_item->m_scalarbarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_titleTextSetting = m_oldTitleTextSetting;
		m_item->m_titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_labelTextSetting = m_oldLabelTextSetting;
		m_item->m_labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);
		Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(m_item->parent());
		tItem->setSize(m_newSize);
		tItem->setColor(m_newCustomColor);

		m_item->setTarget(m_newTarget);
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
		if (m_newTarget != ""){
			LookupTableContainer* lut = gtItem->particleLookupTable(m_newTarget);
			*lut = m_newLookupTable;
			lut->update();
			m_item->m_scalarbarTitleMap[m_newTarget] = m_newScalarBarTitle;
		}
		m_item->m_scalarbarSetting = m_newScalarBarSetting;
		m_item->m_scalarbarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
		m_item->m_titleTextSetting = m_newTitleTextSetting;
		m_item->m_titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_item->m_labelTextSetting = m_newLabelTextSetting;
		m_item->m_labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
		m_item->updateActorSettings();
		m_item->renderGraphicsView();
		m_item->setIsCommandExecuting(false);
	}
private:
	std::string m_oldTarget;
	LookupTableContainer m_oldLookupTable;
	int m_oldSize;
	QColor m_oldCustomColor;

	QString m_oldScalarBarTitle;
	ScalarBarSetting m_oldScalarBarSetting;
	vtkTextPropertySettingContainer m_oldTitleTextSetting;
	vtkTextPropertySettingContainer m_oldLabelTextSetting;

	std::string m_newTarget;
	LookupTableContainer m_newLookupTable;
	int m_newSize;
	QColor m_newCustomColor;

	QString m_newScalarBarTitle;
	ScalarBarSetting m_newScalarBarSetting;
	vtkTextPropertySettingContainer m_newTitleTextSetting;
	vtkTextPropertySettingContainer m_newLabelTextSetting;

	Post2dWindowParticlesScalarGroupDataItem* m_item;
};

void Post2dWindowParticlesScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleScalarPropertyDialog* d = dynamic_cast<PostParticleScalarPropertyDialog*> (propDialog);

	iRICUndoStack::instance().push(new Post2dWindowParticlesScalarSetProperty(d->target(), d->lookupTable(),
																																						d->particleSize(), d->customColor(), d->scalarBarTitle(),
																																						d->scalarBarSetting(), d->titleTextSetting(), d->labelTextSetting(), this));
}

void Post2dWindowParticlesScalarGroupDataItem::informSelection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(1);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initParticleBrowser();
}

void Post2dWindowParticlesScalarGroupDataItem::informDeselection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(0);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearParticleBrowser();
}

void Post2dWindowParticlesScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateParticleBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixParticleBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showParticleBrowserAction();
	menu->addAction(abAction);
}

void Post2dWindowParticlesScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Scalar value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowParticlesScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_scalarbarSetting.visible && v && m_target != "");
	Post2dWindowDataItem::updateVisibility(visible);
}

void Post2dWindowParticlesScalarGroupDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetInteractor(iren);
	m_scalarBarWidget->SetEnabled(0);

	m_scalarbarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
}

void Post2dWindowParticlesScalarGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == 0 || cont->particleData() == 0) {return;}

	Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(parent());

	m_actor->GetProperty()->SetPointSize(tItem->size());
	if (m_target == "") {
		QColor c = tItem->color();
		m_actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
		m_mapper->SetScalarModeToDefault();
	}
	else {
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
		LookupTableContainer* ltc = gtItem->particleLookupTable(m_target);
		m_mapper->SetScalarModeToUsePointFieldData();
		m_mapper->SelectColorArray(m_target.c_str());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SetLookupTable(ltc->vtkObj());

		m_scalarBarWidget->GetScalarBarActor()->SetLookupTable(ltc->vtkObj());
		m_scalarBarWidget->GetScalarBarActor()->SetTitle(iRIC::toStr(m_scalarbarTitleMap[m_target]).c_str());
	}
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent()->parent());
	LookupTableContainer* stc = typedi->particleLookupTable(m_target);
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_scalarbarTitleMap.value(m_target)).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_scalarbarSetting.numberOfLabels);
	a->SetMaximumNumberOfColors(256);
	m_titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_labelTextSetting.applySetting(a->GetLabelTextProperty());
}

void Post2dWindowParticlesScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	setTarget(iRIC::toStr(elem.attribute("solution")));

	m_scalarbarSetting.load(node);
	m_scalarbarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_titleTextSetting.load(node);
	m_labelTextSetting.load(node);

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i) {
		QDomElement titleElem = titles.at(i).toElement();
		std::string val = iRIC::toStr(titleElem.attribute("value"));
		QString title = titleElem.attribute("title");
		m_scalarbarTitleMap[val] = title;
	}
	updateActorSettings();
}

void Post2dWindowParticlesScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", m_target.c_str());

	m_scalarbarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_scalarbarSetting.save(writer);
	m_titleTextSetting.save(writer);
	m_labelTextSetting.save(writer);

	// scalar bar titles
	QMapIterator<std::string, QString> i(m_scalarbarTitleMap);
	while (i.hasNext()) {
		i.next();
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", i.key().c_str());
		writer.writeAttribute("title", i.value());
		writer.writeEndElement();
	}
}
