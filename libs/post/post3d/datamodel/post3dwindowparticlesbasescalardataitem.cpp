#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesbasescalardataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>

#include <vtkActor2D.h>

Post3dWindowParticlesBaseScalarDataItem::Post3dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
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
}

Post3dWindowParticlesBaseScalarDataItem::~Post3dWindowParticlesBaseScalarDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post3dWindowParticlesBaseScalarDataItem::update()
{
	auto range = groupDataItem()->zoneDataItem()->gridTypeDataItem()->particleValueRange(name());
	m_colorMapSetting.setAutoValueRange(range);
}

ColorMapSettingContainer& Post3dWindowParticlesBaseScalarDataItem::colorMapSetting()
{
	return m_colorMapSetting;
}

void Post3dWindowParticlesBaseScalarDataItem::informSelection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleSelection(v);
}

void Post3dWindowParticlesBaseScalarDataItem::informDeselection(VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleDeselection(v);
}

void Post3dWindowParticlesBaseScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void Post3dWindowParticlesBaseScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post3dWindowParticlesBaseScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	m_colorMapSetting.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

bool Post3dWindowParticlesBaseScalarDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

Post3dWindowParticlesBaseScalarGroupDataItem* Post3dWindowParticlesBaseScalarDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowParticlesBaseScalarGroupDataItem*> (parent());
}

void Post3dWindowParticlesBaseScalarDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting.load(node);
}

void Post3dWindowParticlesBaseScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting.save(writer);
}
