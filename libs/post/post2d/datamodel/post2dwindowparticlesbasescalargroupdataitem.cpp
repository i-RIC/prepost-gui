#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalardataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasescalargroupdataitem_setsettingcommand.h"

#include <guibase/vtkCustomScalarBarActor.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainerutil.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guibase/graphicsmisc.h>
#include <postbase/particle/postparticlescalarpropertydialog.h>

#include <QDomElement>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

Post2dWindowParticlesBaseScalarGroupDataItem::Post2dWindowParticlesBaseScalarGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());

	PostZoneDataContainer* cont = topItem->zoneDataItem()->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(topItem->particleData()->GetPointData())){
		auto item = new Post2dWindowParticlesBaseScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
		m_scalarbarTitleMap.insert(name, name.c_str());
	}
}

Post2dWindowParticlesBaseScalarGroupDataItem::~Post2dWindowParticlesBaseScalarGroupDataItem()
{
	vtkRenderer* r = renderer();
	r->RemoveActor(m_actor);
	m_scalarBarWidget->SetInteractor(nullptr);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_actor->SetPosition(0, 0, range.min());
}

void Post2dWindowParticlesBaseScalarGroupDataItem::update()
{
	updateActorSettings();
}

QDialog* Post2dWindowParticlesBaseScalarGroupDataItem::propertyDialog(QWidget* p)
{
	m_setting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.scalarBarSetting.titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	auto tItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	auto gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (tItem->zoneDataItem()->parent());

	auto dialog = new PostParticleScalarPropertyDialog(p);
	dialog->setGridTypeDataItem(gtItem);
	dialog->setData(tItem->particleData());
	dialog->setScalarBarTitleMap(m_scalarbarTitleMap);

	dialog->setSetting(m_setting);
	dialog->setParticleSize(tItem->size());
	dialog->setCustomColor(tItem->color());

	return dialog;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto d = dynamic_cast<PostParticleScalarPropertyDialog*> (propDialog);
	auto cmd = new SetSettingCommand(d->setting(), d->lookupTable(), d->particleSize(), d->customColor(), d->scalarBarTitle(), this);
	pushRenderCommand(cmd, this, true);
}

std::string Post2dWindowParticlesBaseScalarGroupDataItem::target() const
{
	return iRIC::toStr(m_setting.target);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::informSelection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(1);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	topItem->zoneDataItem()->initParticleResultAttributeBrowser(topItem->particleData());
}

void Post2dWindowParticlesBaseScalarGroupDataItem::informDeselection(VTKGraphicsView*)
{
	m_scalarBarWidget->SetRepositionable(0);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	topItem->zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	topItem->zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	topItem->zoneDataItem()->fixParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	menu->addAction(topItem->showAttributeBrowserAction());
}

void Post2dWindowParticlesBaseScalarGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Particle Scalar value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_setting.scalarBarSetting.visible && v && target() != "");
	Post2dWindowDataItem::updateVisibility(visible);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::setupActors()
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

void Post2dWindowParticlesBaseScalarGroupDataItem::updateActorSettings()
{
	m_actor->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	auto data = topItem->particleData();
	if (data == nullptr) {return;}

	auto tItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*>(parent());

	m_actor->GetProperty()->SetPointSize(tItem->size());
	if (target() == "") {
		QColor c = tItem->color();
		m_actor->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
		m_mapper->SetScalarModeToDefault();
	} else {
		auto gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (topItem->zoneDataItem()->parent());
		auto ltc = gtItem->particleLookupTable(target());
		auto dataArray = data->GetPointData()->GetArray(target().c_str());
		ScalarsToColorsContainerUtil::setValueRange(ltc, dataArray);
		m_mapper->SetScalarModeToUsePointFieldData();
		m_mapper->SelectColorArray(iRIC::toStr(m_setting.target).c_str());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SetLookupTable(ltc->vtkObj());

		m_scalarBarWidget->GetScalarBarActor()->SetLookupTable(ltc->vtkObj());
		m_scalarBarWidget->GetScalarBarActor()->SetTitle(iRIC::toStr(m_scalarbarTitleMap[target()]).c_str());
	}
	m_mapper->SetInputData(tItem->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::setupScalarBarSetting()
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent());
	auto typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(topItem->zoneDataItem()->parent());
	auto stc = typedi->particleLookupTable(target());
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_scalarbarTitleMap.value(target())).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_setting.scalarBarSetting.numberOfLabels);
	a->SetMaximumNumberOfColors(256);
	m_setting.scalarBarSetting.titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_setting.scalarBarSetting.labelTextSetting.applySetting(a->GetLabelTextProperty());
}

void Post2dWindowParticlesBaseScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post2dWindowParticlesBaseScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
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
