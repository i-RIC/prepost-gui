#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalardataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

#include <vtkActor2D.h>

Post2dWindowParticlesBaseScalarDataItem::Post2dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent),
	m_colorMapSetting {},
	m_legendActor {vtkActor2D::New()}
{
	renderer()->AddActor2D(m_legendActor);
	m_colorMapSetting.legend.imageSetting.setActor(m_legendActor);
	m_colorMapSetting.legend.imageSetting.controller()->setItem(this);

	auto gtItem = groupDataItem()->topDataItem()->zoneDataItem()->gridTypeDataItem();
	auto gType = gtItem->gridType();
	auto cap = gType->output(name)->caption();
	m_colorMapSetting.valueCaption = cap;
	m_colorMapSetting.legend.title = cap;
	m_colorMapSetting.legend.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::Always;
}

Post2dWindowParticlesBaseScalarDataItem::~Post2dWindowParticlesBaseScalarDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post2dWindowParticlesBaseScalarDataItem::update()
{
	auto range = groupDataItem()->zoneDataItem()->gridTypeDataItem()->particleValueRange(name());
	m_colorMapSetting.setAutoValueRange(range);
}

ColorMapSettingContainer& Post2dWindowParticlesBaseScalarDataItem::colorMapSetting()
{
	return m_colorMapSetting;
}

void Post2dWindowParticlesBaseScalarDataItem::informSelection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
}

void Post2dWindowParticlesBaseScalarDataItem::informDeselection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
}

void Post2dWindowParticlesBaseScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	menu->addAction(topItem->showAttributeBrowserAction());
}

Post2dWindowParticlesBaseScalarGroupDataItem* Post2dWindowParticlesBaseScalarDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseScalarGroupDataItem*> (parent());
}

void Post2dWindowParticlesBaseScalarDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting.load(node);
}

void Post2dWindowParticlesBaseScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting.save(writer);
}
