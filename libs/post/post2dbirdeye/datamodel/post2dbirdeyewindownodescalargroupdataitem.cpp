#include "../post2dbirdeyewindowdatamodel.h"
#include "../post2dbirdeyewindowgraphicsview.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem.h"
#include "post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"
#include "private/post2dbirdeyewindownodescalargroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/graphicswindowdrawcommands.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <vtkGeometryFilter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkWarpScalar.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &colorMode, &customColor, &colorTarget, &opacity},
	regionSetting {},
	colorMode {"colorMode", ColorMode::ByScalar},
	customColor {"customColor", Qt::gray},
	colorTarget {"colorTarget", ""},
	opacity {"opacity", 100}
{}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting& Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Post2dBirdEyeWindowNodeScalarGroupDataItem(const std::string& elevationTarget, Post2dBirdEyeWindowDataItem* p) :
	Post2dBirdEyeWindowDataItem {tr("Scalar"), QIcon(":/libs/guibase/images/iconPaper.svg"), p},
	m_elevationTarget {elevationTarget},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	m_setting.colorMode = Setting::ColorMode::ByScalar;
	m_setting.colorTarget = elevationTarget.c_str();

	auto gType = topDataItem()->zoneDataItem()->dataContainer()->gridType();
	standardItem()->setText(gType->solutionCaption(elevationTarget));

	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	for (const auto& pair : cont->data()->valueRangeSet().pointDataValueRanges()) {
		const auto& name = pair.first;
		auto cs = new ColorMapSettingContainer();
		auto caption = gType->output(name)->caption();
		cs->valueCaption = caption;
		cs->legend.title = caption;
		cs->legend.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::Always;
		cs->setAutoValueRange(pair.second);
		m_colorMapSettings.insert({name, cs});
	}

	setupActors();
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::~Post2dBirdEyeWindowNodeScalarGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_actor);
	r->RemoveActor2D(m_legendActor);

	m_actor->Delete();
	m_legendActor->Delete();

	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}
}

const std::string& Post2dBirdEyeWindowNodeScalarGroupDataItem::elevationTarget() const
{
	return m_elevationTarget;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::updateActorSettings()
{
	m_legendActor->VisibilityOff();
	m_actor2DCollection->RemoveAllItems();

	auto cont = topDataItem()->zoneDataItem()->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}

	auto filtered = m_setting.regionSetting.buildNodeFilteredData(cont->data()->data());
	filtered->GetPointData()->SetActiveScalars(m_elevationTarget.c_str());

	auto warp = vtkSmartPointer<vtkWarpScalar>::New();
	warp->UseNormalOn();
	warp->SetNormal(0, 0, 1);
	warp->SetScaleFactor(1);
	warp->SetInputData(filtered);

	auto filter = vtkSmartPointer<vtkGeometryFilter>::New();
	filter->SetInputConnection(warp->GetOutputPort());
	filter->Update();

	filtered->Delete();
	auto data = filter->GetOutput();

	if (m_setting.colorMode == Setting::ColorMode::Custom) {
		auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
		mapper->SetInputData(data);
		m_actor->SetMapper(mapper);
		mapper->Delete();
	} else if (m_setting.colorMode == Setting::ColorMode::ByScalar) {
		auto cs = activeColorMapSetting();
		if (cs != nullptr) {
			data->GetPointData()->SetActiveScalars(iRIC::toStr(m_setting.colorTarget).c_str());
			auto mapper = cs->buildPointDataMapper(data);
			m_actor->SetMapper(mapper);
			mapper->Delete();
			cs->legend.imageSetting.setActor(m_legendActor);
			cs->legend.imageSetting.controller()->setItem(this);
			auto v = dataModel()->graphicsView();
			cs->legend.imageSetting.apply(v);
		}
	}

	m_actor->GetProperty()->SetOpacity(m_setting.opacity);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::setupActors()
{
	auto r = renderer();
	r->AddActor(m_actor);
	r->AddActor2D(m_legendActor);

	m_actorCollection->AddItem(m_actor);

	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::update()
{
	auto gtItem = topDataItem()->zoneDataItem()->gridTypeDataItem();
	for (const auto& pair : m_colorMapSettings) {
		auto range = gtItem->nodeValueRange(pair.first);
		pair.second->setAutoValueRange(range);
	}

	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::showPropertyDialog()
{
	return showPropertyDialogModeless();
}

QDialog* Post2dBirdEyeWindowNodeScalarGroupDataItem::propertyDialog(QWidget* p)
{
	auto dialog = new PropertyDialog(this, p);
	dialog->setWindowTitle(tr("Scalar Setting (%1)").arg(standardItem()->text()));
	return dialog;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::innerUpdateZScale(double scale)
{
	m_actor->SetScale(1, 1, scale);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::informSelection(VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleSelection(v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleDeselection(v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMousePressEvent(event, v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doHandleResize(VTKGraphicsView* v)
{
	auto cs = activeColorMapSetting();
	if (cs != nullptr) {
		cs->legend.imageSetting.controller()->handleResize(v);
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);

	auto childNodes = node.childNodes();
	for (int i = 0; i < childNodes.size(); ++i) {
		auto child = childNodes.at(i);
		if (child.nodeName() == "ColorMapSetting") {
			auto elem = child.toElement();
			auto name = iRIC::toStr(elem.attribute("name"));
			auto it = m_colorMapSettings.find(name);
			if (it == m_colorMapSettings.end()) {continue;}

			it->second->load(child);
		}
	}

	updateActorSettings();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);

	for (const auto& pair : m_colorMapSettings) {
		writer.writeStartElement("ColorMapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
}

Post2dBirdEyeWindowNodeScalarGroupTopDataItem* Post2dBirdEyeWindowNodeScalarGroupDataItem::topDataItem() const
{
	return dynamic_cast<Post2dBirdEyeWindowNodeScalarGroupTopDataItem*> (parent());
}

ColorMapSettingContainer* Post2dBirdEyeWindowNodeScalarGroupDataItem::colorMapSetting(const std::string& name) const
{
	auto it = m_colorMapSettings.find(name);
	if (it == m_colorMapSettings.end()) {return nullptr;}

	return it->second;
}

ColorMapSettingContainer* Post2dBirdEyeWindowNodeScalarGroupDataItem::activeColorMapSetting() const
{
	if (m_setting.colorMode == Setting::ColorMode::Custom) {return nullptr;}

	return colorMapSetting(iRIC::toStr(m_setting.colorTarget));
}
