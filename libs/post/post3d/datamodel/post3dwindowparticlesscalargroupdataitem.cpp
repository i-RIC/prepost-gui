#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesscalargroupdataitem.h"
#include "post3dwindowparticlesscalardataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowparticlesscalargroupdataitem_setsettingcommand.h"

#include <guibase/vtkCustomScalarBarActor.h>
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
#include <QStandardItem>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

Post3dWindowParticlesScalarGroupDataItem::Post3dWindowParticlesScalarGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->particleData()->GetPointData())){
		auto item = new Post3dWindowParticlesScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
		m_scalarbarTitleMap.insert(name, name.c_str());
	}
}

Post3dWindowParticlesScalarGroupDataItem::~Post3dWindowParticlesScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_actor);
	m_scalarBarWidget->SetInteractor(0);
}

void Post3dWindowParticlesScalarGroupDataItem::update()
{
	updateActorSettings();
}

QDialog* Post3dWindowParticlesScalarGroupDataItem::propertyDialog(QWidget* p)
{
	Post3dWindowParticlesTopDataItem* tItem =
			dynamic_cast<Post3dWindowParticlesTopDataItem*> (parent());

	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.scalarBarSetting.titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	PostParticleScalarPropertyDialog* dialog = new PostParticleScalarPropertyDialog(p);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	dialog->setZoneData(cont);
	dialog->setScalarBarTitleMap(m_scalarbarTitleMap);

	dialog->setSetting(m_setting);
	dialog->setParticleSize(tItem->size());
	dialog->setCustomColor(tItem->color());

	return dialog;
}

void Post3dWindowParticlesScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleScalarPropertyDialog* d = dynamic_cast<PostParticleScalarPropertyDialog*> (propDialog);

	auto cmd = new SetSettingCommand(d->setting(), d->lookupTable(), d->particleSize(), d->customColor(), d->scalarBarTitle(), this);
	pushRenderCommand(cmd, this, true);
}

std::string Post3dWindowParticlesScalarGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post3dWindowParticlesScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
}

void Post3dWindowParticlesScalarGroupDataItem::informSelection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(1);
}

void Post3dWindowParticlesScalarGroupDataItem::informDeselection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(0);
}

void Post3dWindowParticlesScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post3dWindowParticlesScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post3dWindowParticlesScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void Post3dWindowParticlesScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Scalar value change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowParticlesScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible && v && target() != "");
	Post3dWindowDataItem::updateVisibility(visible);
}

void Post3dWindowParticlesScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post3dWindowParticlesScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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

void Post3dWindowParticlesScalarGroupDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_actor);
	m_actor->GetProperty()->LightingOff();

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_actor->SetMapper(m_mapper);

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();

	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	m_scalarBarWidget->SetScalarBarActor(vtkCustomScalarBarActor::New());
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetInteractor(iren);
	m_scalarBarWidget->SetEnabled(0);

	m_setting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	updateActorSettings();
}

void Post3dWindowParticlesScalarGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == 0 || cont->particleData() == 0) {return;}

	Post3dWindowParticlesTopDataItem* tItem = dynamic_cast<Post3dWindowParticlesTopDataItem*>(parent());

	m_actor->GetProperty()->SetPointSize(tItem->size());
	if (target() == "") {
		QColor c = tItem->color();
		m_actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
		m_mapper->SetScalarModeToDefault();
	}	else {
		Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (parent()->parent()->parent());
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

void Post3dWindowParticlesScalarGroupDataItem::setupScalarBarSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
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

void Post3dWindowParticlesScalarGroupDataItem::innerUpdateZScale(double zscale)
{
	m_actor->SetScale(1, 1, zscale);
}
