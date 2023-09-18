#include "preprocessorgriddataitem.h"
#include "preprocessorunstructured2dgridshapedataitem.h"
#include "public/preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/graphicsmisc.h>
#include <guibase/gridshape/gridshapesettingeditwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

PreProcessorUnstructured2dGridShapeDataItem::PreProcessorUnstructured2dGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridShapeDataItem {parent}
{
	setupActors();
	updateActorSetting();
}

PreProcessorUnstructured2dGridShapeDataItem::~PreProcessorUnstructured2dGridShapeDataItem()
{
	renderer()->RemoveActor(m_setting.wireframeActor());
	renderer()->RemoveActor2D(m_setting.indexActor());
}

void PreProcessorUnstructured2dGridShapeDataItem::setupActors()
{
	auto wa = m_setting.wireframeActor();
	wa->GetProperty()->SetLighting(false);
	renderer()->AddActor(wa);

	auto ia = m_setting.indexActor();
	renderer()->AddActor2D(ia);
}


void PreProcessorUnstructured2dGridShapeDataItem::informGridUpdate()
{
	updateActorSetting();
}

void PreProcessorUnstructured2dGridShapeDataItem::updateActorSetting()
{
	actorCollection()->RemoveAllItems();
	actor2DCollection()->RemoveAllItems();

	m_setting.outlineActor()->VisibilityOff();
	m_setting.wireframeActor()->VisibilityOff();
	m_setting.indexActor()->VisibilityOff();

	v4InputGrid* g = gridDataItem()->grid();
	if (g == nullptr) {return;}

	auto grid2d = dynamic_cast<v4Grid2d*> (g->grid());
	vtkPointSet* filteredData = grid2d->vtkFilteredData();
	vtkPointSet* indexData = grid2d->vtkFilteredIndexData();

	m_setting.update(actorCollection(), actor2DCollection(),
									 filteredData, filteredData,
									 indexData, v4GridUtil::LABEL_NAME);

	updateVisibilityWithoutRendering();
}

void PreProcessorUnstructured2dGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* PreProcessorUnstructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new GridShapeSettingEditWidget(dialog);
	dialog->setWidget(widget);

	widget->hideShape();
	widget->setSetting(&m_setting);
	dialog->setWindowTitle(tr("Grid Shape Setting"));

	return dialog;
}

void PreProcessorUnstructured2dGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	gridDataItem()->selectedNodesController()->setVisibility(true);
	updateVisibility();
}

void PreProcessorUnstructured2dGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	gridDataItem()->selectedNodesController()->setVisibility(false);
}

void PreProcessorUnstructured2dGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_setting.wireframeActor()->SetPosition(0, 0, range.min());
}
