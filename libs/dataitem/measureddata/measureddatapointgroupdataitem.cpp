#include "measureddatafiledataitem.h"
#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "private/measureddatapointgroupdataitem_impl.h"
#include "private/measureddatapointgroupdataitem_propertydialog.h"

#include <guibase/graphicsmisc.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontainer.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkActor.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

MeasuredDataPointGroupDataItem::Impl::Impl(MeasuredDataPointGroupDataItem* item) :
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_item {item}
{}

MeasuredDataPointGroupDataItem::Impl::~Impl()
{
	vtkRenderer* r = m_item->renderer();

	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();
}

void MeasuredDataPointGroupDataItem::Impl::setupActors()
{
	vtkRenderer* r = m_item->renderer();

	m_actor->GetProperty()->SetLighting(false);
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	updateActorSettings();
}

void MeasuredDataPointGroupDataItem::Impl::updateCheckState()
{
	if (m_setting.mappingMode == MeasuredDataPointSetting::MappingMode::Arbitrary) {
		NamedGraphicsWindowDataItemTool::checkItemWithName("", m_item->m_childItems, true);
	} else {
		NamedGraphicsWindowDataItemTool::checkItemWithName(iRIC::toStr(m_setting.value), m_item->m_childItems, true);
	}
}

void MeasuredDataPointGroupDataItem::Impl::updateActorSettings()
{
	// make all the items invisible
	m_actor->VisibilityOff();
	m_item->m_actorCollection->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(m_item->parent())->measuredData();
	vtkPolyData* polyData = nullptr;
	if (m_setting.shapeMode == MeasuredDataPointSetting::ShapeMode::Points) {
		polyData = md->pointData();
	} else if (m_setting.shapeMode == MeasuredDataPointSetting::ShapeMode::Surface) {
		polyData = md->polyData();
	}

	if (m_setting.mappingMode == MeasuredDataPointSetting::MappingMode::Arbitrary) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(polyData);
		m_actor->SetMapper(mapper);
		mapper->Delete();

		m_actor->GetProperty()->SetColor(m_setting.color);
	} else {
		auto value = iRIC::toStr(m_setting.value);
		auto cs = m_colorMapSettings.at(value);
		auto mapper = cs->buildPointDataMapper(polyData);
		m_actor->SetMapper(mapper);
		mapper->Delete();
	}

	auto prop = m_actor->GetProperty();
	prop->SetOpacity(m_setting.opacity);
	prop->SetPointSize(m_setting.pointSize);

	m_item->m_actorCollection->AddItem(m_actor);

	m_item->assignActorZValues(m_item->m_zDepthRange);
	m_item->updateVisibilityWithoutRendering();

	auto as = m_item->activeSetting();
	if (as != nullptr) {
		auto v = m_item->dataModel()->graphicsView();
		as->legend.imageSetting.controller()->handleSelection(v);
	} else {
		m_legendActor->VisibilityOff();
	}
}

// public interfaces

MeasuredDataPointGroupDataItem::MeasuredDataPointGroupDataItem(GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->setupActors();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(p)->measuredData();

	for (int i = 0; i < md->scalarNames().size(); ++i) {
		std::string name = md->scalarNames().at(i);
		MeasuredDataPointDataItem* item = new MeasuredDataPointDataItem(name, name.c_str(), this);
		m_childItems.push_back(item);

		auto s = new ColorMapSettingContainer();
		vtkDataArray* da = md->polyData()->GetPointData()->GetArray(name.c_str());
		double range[2];
		da->GetRange(range);
		s->valueCaption = name.c_str();
		s->legend.setTitle(name.c_str());
		s->setAutoValueRange(range[0], range[1]);
		s->legend.imageSetting.setActor(impl->m_legendActor);
		s->legend.imageSetting.controller()->setItem(this);

		impl->m_colorMapSettings.insert({name, s});
	}

	if (md->scalarNames().size() > 0) {
		auto name = md->scalarNames().at(0);
		impl->m_setting.mappingMode = MeasuredDataPointSetting::MappingMode::Value;
		setTarget(name);
	}
}

MeasuredDataPointGroupDataItem::~MeasuredDataPointGroupDataItem()
{
	delete impl;
}

void MeasuredDataPointGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);

	auto cmNode = iRIC::getChildNode(node, "ColorMaps");
	if (! cmNode.isNull()) {
		for (int i = 0; i < cmNode.childNodes().count(); ++i) {
			QDomElement elem = cmNode.childNodes().at(i).toElement();
			std::string name = iRIC::toStr(elem.attribute("name"));

			auto cs = colorMapSetting(name);
			if (cs == nullptr) {continue;}
			cs->load(elem);
		}
	}

	impl->updateActorSettings();
}

void MeasuredDataPointGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);

	writer.writeStartElement("ColorMaps");
	for (auto pair : impl->m_colorMapSettings) {
		writer.writeStartElement("ColorMap");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

ColorMapSettingContainer* MeasuredDataPointGroupDataItem::activeSetting() const
{
	if (impl->m_setting.mappingMode == MeasuredDataPointSetting::MappingMode::Arbitrary) {return nullptr;}

	auto v = iRIC::toStr(impl->m_setting.value);
	auto it = impl->m_colorMapSettings.find(v);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

void MeasuredDataPointGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataPointGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_actor->SetPosition(0, 0, range.max());
}

void MeasuredDataPointGroupDataItem::update()
{
	impl->updateActorSettings();
}

QDialog* MeasuredDataPointGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return nullptr;
	}

	return new PropertyDialog(this, p);
}

void MeasuredDataPointGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<PropertyDialog*> (propDialog);
	pushRenderCommand(dialog->createModifyCommand(), this, true);
}

void MeasuredDataPointGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Contour Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

std::string MeasuredDataPointGroupDataItem::target() const
{
	return iRIC::toStr(impl->m_setting.value);
}

void MeasuredDataPointGroupDataItem::setTarget(const std::string &target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_setting.value = target.c_str();

	if (target == "") {
		impl->m_setting.mappingMode = MeasuredDataPointSetting::MappingMode::Arbitrary;
	} else {
		impl->m_setting.mappingMode = MeasuredDataPointSetting::MappingMode::Value;
	}

	impl->updateActorSettings();
}

bool MeasuredDataPointGroupDataItem::hasTransparentPart()
{
	if (standardItem()->checkState() == Qt::Unchecked) {return false;}
	if (impl->m_setting.value == "") {return false;}
	if (impl->m_setting.opacity.value() == 100) {return false;}
	return true;
}

ColorMapSettingContainer* MeasuredDataPointGroupDataItem::colorMapSetting(const std::string& target) const
{
	auto it = impl->m_colorMapSettings.find(target);
	if (it == impl->m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

std::unordered_map<std::string, ColorMapSettingContainer*> MeasuredDataPointGroupDataItem::colorMapSettings() const
{
	return impl->m_colorMapSettings;
}

void MeasuredDataPointGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
}

void MeasuredDataPointGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
}

void MeasuredDataPointGroupDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleResize(event, v);
}

void MeasuredDataPointGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataPointGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void MeasuredDataPointGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

void MeasuredDataPointGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	impl->updateActorSettings();
}
