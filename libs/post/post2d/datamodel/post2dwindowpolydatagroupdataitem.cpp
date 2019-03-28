#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatavaluedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowpolydatagroupdataitem_setbasicsettingcommand.h"
#include "private/post2dwindowpolydatagroupdataitem_setsettingcommand.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <misc/stringtool.h>
#include <postbase/polydata/postpolydatabasicsettingdialog.h>
#include <postbase/polydata/postpolydatascalarpropertydialog.h>

#include <QDomNodeList>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarWidget.h>

Post2dWindowPolyDataGroupDataItem::Post2dWindowPolyDataGroupDataItem(const std::string& name, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {name.c_str(), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	m_scalarBarWidget {vtkScalarBarWidget::New()}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	auto data = cont->polyData(name);
	auto cd = data->GetCellData();
	int num = cd->GetNumberOfArrays();

	SolverDefinitionGridType* gt = cont->gridType();
	for (int i = 0; i < num; ++i) {
		auto a = cd->GetArray(i);
		if (a == nullptr) {continue;}

		auto item = new Post2dWindowPolyDataValueDataItem(a->GetName(), gt->solutionCaption(std::string(a->GetName())), this);
		m_childItems.push_back(item);
		m_scalarbarTitleMap.insert({a->GetName(), a->GetName()});
	}
	setupActors();
}

Post2dWindowPolyDataGroupDataItem::~Post2dWindowPolyDataGroupDataItem()
{
	renderer()->RemoveActor(m_pair.actor());
	m_scalarBarWidget->SetInteractor(nullptr);
	m_scalarBarWidget->Delete();
}

std::string Post2dWindowPolyDataGroupDataItem::name() const
{
	return iRIC::toStr(m_standardItem->text());
}

QColor Post2dWindowPolyDataGroupDataItem::color() const
{
	return m_setting.color;
}

void Post2dWindowPolyDataGroupDataItem::setColor(const QColor& c)
{
	m_setting.color = c;
}

std::string Post2dWindowPolyDataGroupDataItem::target() const
{
	return std::string(m_scalarSetting.target);
}

void Post2dWindowPolyDataGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_scalarSetting.target = target.c_str();
	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::setupActors()
{
	renderer()->AddActor(m_pair.actor());

	m_scalarBarWidget->SetScalarBarActor(vtkCustomScalarBarActor::New());
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	auto iren = renderer()->GetRenderWindow()->GetInteractor();
	m_scalarBarWidget->SetInteractor(iren);
	m_scalarBarWidget->SetEnabled(0);
	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::update()
{
	updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::updateActorSettings()
{
	m_pair.actor()->VisibilityOff();
	m_actorCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->polyData(name()) == nullptr) {return;}

	if (target() == "") {
		QColor c = m_setting.color;
		m_pair.actor()->GetProperty()->SetColor(c.redF(), c.greenF(), c.blueF());
		auto mapper = m_pair.mapper();
		mapper->ScalarVisibilityOff();
		mapper->SetScalarModeToDefault();
	} else {
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
		LookupTableContainer* ltc = gtItem->polyDataLookupTable(target());
		auto mapper = m_pair.mapper();
		mapper->ScalarVisibilityOn();
		mapper->SetScalarModeToUseCellFieldData();
		mapper->SelectColorArray(iRIC::toStr(m_scalarSetting.target).c_str());
		mapper->UseLookupTableScalarRangeOn();
		mapper->SetLookupTable(ltc->vtkObj());

		m_scalarBarWidget->GetScalarBarActor()->SetLookupTable(ltc->vtkObj());
		m_scalarBarWidget->GetScalarBarActor()->SetTitle(iRIC::toStr(m_scalarbarTitleMap[target()]).c_str());
	}
	m_pair.actor()->GetProperty()->SetLineWidth(m_setting.lineWidth);
	m_pair.mapper()->SetInputData(cont->polyData(name()));

	m_actorCollection->AddItem(m_pair.actor());
	updateVisibilityWithoutRendering();
}

QDialog* Post2dWindowPolyDataGroupDataItem::propertyDialog(QWidget* p)
{
	if (m_childItems.size() > 0) {
		m_scalarSetting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
		m_scalarSetting.scalarBarSetting.titleTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
		m_scalarSetting.scalarBarSetting.labelTextSetting.getSetting(m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

		auto dialog = new PostPolyDataScalarPropertyDialog(p);
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*> (parent()->parent()->parent());
		dialog->setGridTypeDataItem(gtItem);
		PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->dataContainer();
		vtkPolyData* polyData = cont->polyData(name());
		dialog->setPolyData(polyData);
		dialog->setScalarBarTitleMap(m_scalarbarTitleMap);

		dialog->setSetting(m_scalarSetting);
		dialog->setLineWidth(m_setting.lineWidth);
		dialog->setCustomColor(m_setting.color);
		return dialog;
	} else {
		auto dialog = new PostPolyDataBasicSettingDialog(mainWindow());
		dialog->setSetting(m_setting);
		return dialog;
	}
}

void Post2dWindowPolyDataGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	if (m_childItems.size() > 0) {
		auto d = dynamic_cast<PostPolyDataScalarPropertyDialog*>(propDialog);
		pushRenderCommand(new SetSettingCommand(d->setting(), d->lookupTable(), d->lineWidgh(), d->customColor(), d->scalarBarTitle(), this), this, true);
	} else {
		auto d = dynamic_cast<PostPolyDataBasicSettingDialog*>(propDialog);
		pushRenderCommand(new SetBasicSettingCommand(d->setting(), this), this);
	}
}

void Post2dWindowPolyDataGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowPolyDataGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_pair.actor()->SetPosition(0, 0, range.min());
}

void Post2dWindowPolyDataGroupDataItem::informSelection(VTKGraphicsView* v)
{
	m_scalarBarWidget->SetRepositionable(1);
	zoneDataItem()->initPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	m_scalarBarWidget->SetRepositionable(0);
	zoneDataItem()->clearPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	zoneDataItem()->updatePolyDataResultAttributeBrowser(name(), event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowPolyDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	zoneDataItem()->fixPolyDataResultAttributeBrowser(name(), event->pos(), v);
}

void Post2dWindowPolyDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* a = zoneDataItem()->showAttributeBrowserActionForPolyDataResult();
	menu->addAction(a);
}

void Post2dWindowPolyDataGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("PolyData value change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowPolyDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	m_scalarSetting.load(node);
	m_scalarSetting.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());

	QDomNodeList titles = node.childNodes();
	for (int i = 0; i < titles.count(); ++i) {
		QDomElement titleElem = titles.at(i).toElement();
		std::string val = iRIC::toStr(titleElem.attribute("value"));
		QString title = titleElem.attribute("title");
		m_scalarbarTitleMap[val] = title;
	}
	setTarget(iRIC::toStr(m_scalarSetting.target));
}

void Post2dWindowPolyDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_scalarSetting.scalarBarSetting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
	m_setting.save(writer);
	m_scalarSetting.save(writer);

	for (const auto& pair : m_scalarbarTitleMap) {
		writer.writeStartElement("ScalarBarTitle");
		writer.writeAttribute("value", pair.first.c_str());
		writer.writeAttribute("title", pair.second);
		writer.writeEndElement();
	}
}

void Post2dWindowPolyDataGroupDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(m_scalarSetting.scalarBarSetting.visible && v && target() != "");
	Post2dWindowDataItem::updateVisibility(visible);
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataGroupDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent());
}
