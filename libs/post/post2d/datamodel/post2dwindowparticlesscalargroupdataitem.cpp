#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesscalargroupdataitem.h"
#include "post2dwindowparticlesscalardataitem.h"
#include "post2dwindowparticlestopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesscalargroupdataitem_setsettingcommand.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guibase/graphicsmisc.h>
#include <postbase/particle/postparticlescalarpropertydialog.h>

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
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->particleData()->GetPointData())){
		auto item = new Post2dWindowParticlesScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
		m_scalarbarTitleMap.insert(name, name.c_str());
	}
}

Post2dWindowParticlesScalarGroupDataItem::~Post2dWindowParticlesScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_actor);
	m_scalarBarWidget->SetInteractor(0);
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

	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.scalarBarSetting.titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	PostParticleScalarPropertyDialog* dialog = new PostParticleScalarPropertyDialog(p);
	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	dialog->setZoneData(cont);
	dialog->setScalarBarTitleMap(m_scalarbarTitleMap);

	dialog->setSetting(m_setting);
	dialog->setParticleSize(tItem->size());
	dialog->setCustomColor(tItem->color());

	return dialog;
}

void Post2dWindowParticlesScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleScalarPropertyDialog* d = dynamic_cast<PostParticleScalarPropertyDialog*> (propDialog);

	auto cmd = new SetSettingCommand(d->setting(), d->lookupTable(), d->particleSize(), d->customColor(), d->scalarBarTitle(), this);
	pushRenderCommand(cmd, this, true);
}

std::string Post2dWindowParticlesScalarGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post2dWindowParticlesScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
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
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible && v && target() != "");
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

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
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
	if (target() == "") {
		QColor c = tItem->color();
		m_actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
		m_mapper->SetScalarModeToDefault();
	} else {
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
		LookupTableContainer* ltc = gtItem->particleLookupTable(target());
		m_mapper->SetScalarModeToUsePointFieldData();
		m_mapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SetLookupTable(ltc->vtkObj());

		m_scalarBarWidget->GetScalarBarActor()->SetLookupTable(ltc->vtkObj());
		m_scalarBarWidget->GetScalarBarActor()->SetTitle(iRIC::toStr(m_scalarbarTitleMap[target()]).c_str());
	}
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesScalarGroupDataItem::setupScalarBarSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent()->parent());
	LookupTableContainer* stc = typedi->particleLookupTable(target());
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_scalarbarTitleMap.value(target())).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_setting.scalarBarSetting.numberOfLabels);
	a->SetMaximumNumberOfColors(256);
	m_setting.scalarBarSetting.titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.applySetting(a->GetLabelTextProperty());
}

void Post2dWindowParticlesScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i) {
		QDomElement titleElem = titles.at(i).toElement();
		std::string val = iRIC::toStr(titleElem.attribute("value"));
		QString title = titleElem.attribute("title");
		m_scalarbarTitleMap[val] = title;
	}
	updateActorSettings();

	setTarget(iRIC::toStr(m_setting.target));
}

void Post2dWindowParticlesScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.save(writer);

	// scalar bar titles
	QMapIterator<std::string, QString> i(m_scalarbarTitleMap);
	while (i.hasNext()) {
		i.next();
		if (i.key().size() > 0) {
			writer.writeStartElement("ScalarBarTitle");
			writer.writeAttribute("value", i.key().c_str());
			writer.writeAttribute("title", i.value());
			writer.writeEndElement();
		}
	}
}
