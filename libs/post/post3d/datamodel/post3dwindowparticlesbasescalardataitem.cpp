#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesbasescalardataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkActor2D.h>

Post3dWindowParticlesBaseScalarDataItem::Post3dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent),
	m_colorMapSetting {nullptr},
	m_legendActor {vtkActor2D::New()}
{
	renderer()->AddActor2D(m_legendActor);

	auto gtItem = groupDataItem()->topDataItem()->zoneDataItem()->gridTypeDataItem();
	auto gType = gtItem->gridType();
	auto output = gType->output(name);
	m_colorMapSetting = output->createColorMapSettingContainer();
	m_colorMapSetting->valueCaption = output->caption();
	m_colorMapSetting->legendSetting()->setTitle(output->caption());
	m_colorMapSetting->legendSetting()->imgSetting()->setActor(m_legendActor);
}

Post3dWindowParticlesBaseScalarDataItem::~Post3dWindowParticlesBaseScalarDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post3dWindowParticlesBaseScalarDataItem::update()
{
	auto range = groupDataItem()->zoneDataItem()->gridTypeDataItem()->particleValueRange(name());
	m_colorMapSetting->setAutoValueRange(range);
}

ColorMapSettingContainerI* Post3dWindowParticlesBaseScalarDataItem::colorMapSetting() const
{
	return m_colorMapSetting;
}

void Post3dWindowParticlesBaseScalarDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post3dWindowParticlesBaseScalarDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

void Post3dWindowParticlesBaseScalarDataItem::informSelection(VTKGraphicsView* /*v*/)
{}

void Post3dWindowParticlesBaseScalarDataItem::informDeselection(VTKGraphicsView* /*v*/)
{}

void Post3dWindowParticlesBaseScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowParticlesBaseScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowParticlesBaseScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
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
	m_colorMapSetting->load(node);
}

void Post3dWindowParticlesBaseScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting->save(writer);
}
