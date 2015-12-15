#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesscalargroupdataitem.h"
#include "post3dwindowparticlesscalardataitem.h"
#include "post3dwindowparticlestopdataitem.h"
#include "post3dwindowzonedataitem.h"

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
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkPointData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActor.h>

Post3dWindowParticlesScalarGroupDataItem::Post3dWindowParticlesScalarGroupDataItem(Post3dWindowDataItem* p)
	: Post3dWindowDataItem(tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.png"), p)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setDefaultValues();
	setupActors();
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
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
		Post3dWindowParticlesScalarDataItem* item = new Post3dWindowParticlesScalarDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
		m_scalarbarTitleMap.insert(name, name.c_str());
	}
	m_titleTextSetting.setPrefix("titleText");
	m_labelTextSetting.setPrefix("labelText");
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

	m_scalarbarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	PostParticleScalarPropertyDialog* dialog = new PostParticleScalarPropertyDialog(p);
	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (parent()->parent()->parent());
	dialog->setGridTypeDataItem(gtItem);
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
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

class Post3dWindowParticlesScalarSetProperty : public QUndoCommand
{
public:
	Post3dWindowParticlesScalarSetProperty(const std::string& sol, const LookupTableContainer& ltc, int size, const QColor& color, QString colorbarTitle, ScalarBarSetting scalarBarSetting, const vtkTextPropertySettingContainer& titleCont, const vtkTextPropertySettingContainer& labelCont, Post3dWindowParticlesScalarGroupDataItem* item) :
		QUndoCommand(QObject::tr("Update Particles Scalar Setting"))
	{
		m_newTarget = sol;
		m_newLookupTable = ltc;
		m_newScalarBarTitle = colorbarTitle;
		m_newSize = size;
		m_newCustomColor = color;
		m_newScalarBarSetting = scalarBarSetting;
		m_newTitleTextSetting = titleCont;
		m_newLabelTextSetting = labelCont;

		m_oldTarget = item->m_target;
		if (m_newTarget != ""){
			Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(item->parent()->parent()->parent());
			LookupTableContainer* lut = gtItem->particleLookupTable(m_newTarget);
			m_oldLookupTable = *lut;

			m_oldScalarBarTitle = item->m_scalarbarTitleMap.value(sol);
		}

		Post3dWindowParticlesTopDataItem* tItem = dynamic_cast<Post3dWindowParticlesTopDataItem*>(item->parent());
		m_oldSize = tItem->size();
		m_oldCustomColor = tItem->color();
		m_oldScalarBarSetting = item->m_scalarbarSetting;
		m_oldTitleTextSetting = item->m_titleTextSetting;
		m_oldLabelTextSetting = item->m_labelTextSetting;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);
		Post3dWindowParticlesTopDataItem* tItem = dynamic_cast<Post3dWindowParticlesTopDataItem*>(m_item->parent());
		tItem->setSize(m_oldSize);
		tItem->setColor(m_oldCustomColor);

		m_item->setTarget(m_oldTarget);
		Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
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
		Post3dWindowParticlesTopDataItem* tItem = dynamic_cast<Post3dWindowParticlesTopDataItem*>(m_item->parent());
		tItem->setSize(m_newSize);
		tItem->setColor(m_newCustomColor);

		m_item->setTarget(m_newTarget);
		Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
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

	Post3dWindowParticlesScalarGroupDataItem* m_item;
};

void Post3dWindowParticlesScalarGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleScalarPropertyDialog* d = dynamic_cast<PostParticleScalarPropertyDialog*> (propDialog);

	iRICUndoStack::instance().push(new Post3dWindowParticlesScalarSetProperty(d->target(), d->lookupTable(),
																																						d->particleSize(), d->customColor(), d->scalarBarTitle(),
																																						d->scalarBarSetting(), d->titleTextSetting(), d->labelTextSetting(), this));
}

std::string Post3dWindowParticlesScalarGroupDataItem::target() const
{
	return m_target;
}

void Post3dWindowParticlesScalarGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_target = target;
	updateActorSettings();
}

void Post3dWindowParticlesScalarGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
}

void Post3dWindowParticlesScalarGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
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
	m_scalarBarWidget->SetEnabled(m_scalarbarSetting.visible && v && target() != "");
	Post3dWindowDataItem::updateVisibility(visible);
}

void Post3dWindowParticlesScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
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

void Post3dWindowParticlesScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("solution", target().c_str());

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

void Post3dWindowParticlesScalarGroupDataItem::setupActors()
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
		m_mapper->SelectColorArray(m_target.c_str());
		m_mapper->UseLookupTableScalarRangeOn();
		m_mapper->SetLookupTable(ltc->vtkObj());

		m_scalarBarWidget->GetScalarBarActor()->SetLookupTable(ltc->vtkObj());
		m_scalarBarWidget->GetScalarBarActor()->SetTitle(iRIC::toStr(m_scalarbarTitleMap[target()]).c_str());
	}
	m_mapper->SetInputData(cont->particleData());

	m_actorCollection->AddItem(m_actor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowParticlesScalarGroupDataItem::setDefaultValues()
{

}

void Post3dWindowParticlesScalarGroupDataItem::setupScalarBarSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	LookupTableContainer* stc = typedi->particleLookupTable(target());
	if (stc == nullptr) {return;}

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_scalarbarTitleMap.value(target())).c_str());
	a->SetLookupTable(stc->vtkObj());
	a->SetNumberOfLabels(m_scalarbarSetting.numberOfLabels);
	a->SetMaximumNumberOfColors(256);
	m_titleTextSetting.applySetting(a->GetTitleTextProperty());
	m_labelTextSetting.applySetting(a->GetLabelTextProperty());
}

void Post3dWindowParticlesScalarGroupDataItem::innerUpdateZScale(double zscale)
{
	m_actor->SetScale(1, 1, zscale);
}
