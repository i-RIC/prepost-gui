#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowparticlesbasescalardataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowParticlesBaseScalarDataItem::Post2dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent),
	m_colorMapSetting {nullptr},
	m_legendActor {vtkActor2D::New()}
{
	renderer()->AddActor2D(m_legendActor);

	auto gtItem = groupDataItem()->topDataItem()->zoneDataItem()->gridTypeDataItem();
	auto gType = gtItem->gridType();
	auto output = gType->output(name);
	m_colorMapSetting = output->createColorMapSettingContainer();
	m_colorMapSetting->valueCaption = caption;
	m_colorMapSetting->legendSetting()->setTitle(caption);
	m_colorMapSetting->legendSetting()->imgSetting()->setActor(m_legendActor);
}

Post2dWindowParticlesBaseScalarDataItem::~Post2dWindowParticlesBaseScalarDataItem()
{
	renderer()->RemoveActor2D(m_legendActor);
	m_legendActor->Delete();
}

void Post2dWindowParticlesBaseScalarDataItem::update()
{
	auto range = groupDataItem()->zoneDataItem()->gridTypeDataItem()->particleValueRange(name());
	m_colorMapSetting->setAutoValueRange(range);
}

ColorMapSettingContainerI* Post2dWindowParticlesBaseScalarDataItem::colorMapSetting() const
{
	return m_colorMapSetting;
}

void Post2dWindowParticlesBaseScalarDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowParticlesBaseScalarDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

void Post2dWindowParticlesBaseScalarDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post2dWindowParticlesBaseScalarDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post2dWindowParticlesBaseScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

void Post2dWindowParticlesBaseScalarDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = groupDataItem()->topDataItem();
	menu->addAction(topItem->showAttributeBrowserAction());
}

bool Post2dWindowParticlesBaseScalarDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

Post2dWindowParticlesBaseScalarGroupDataItem* Post2dWindowParticlesBaseScalarDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseScalarGroupDataItem*> (parent());
}

void Post2dWindowParticlesBaseScalarDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_colorMapSetting->load(node);
}

void Post2dWindowParticlesBaseScalarDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_colorMapSetting->save(writer);
}
