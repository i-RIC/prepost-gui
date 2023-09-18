#include "preprocessorgriddataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"
#include "public/preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/graphicsmisc.h>
#include <guibase/gridshape/gridshapesettingeditwidget.h>
#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4gridutil.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guibase/gridshape/gridshapesettingeditwidget.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

PreProcessorStructured2dGridShapeDataItem::PreProcessorStructured2dGridShapeDataItem(PreProcessorDataItem* parent) :
	PreProcessorGridShapeDataItem {parent}
{
	setupActors();
	updateActorSetting();
}

PreProcessorStructured2dGridShapeDataItem::~PreProcessorStructured2dGridShapeDataItem()
{
	renderer()->RemoveActor(m_setting.outlineActor());
	renderer()->RemoveActor(m_setting.wireframeActor());
	renderer()->RemoveActor2D(m_setting.indexActor());
}

void PreProcessorStructured2dGridShapeDataItem::setupActors()
{
	auto oa = m_setting.outlineActor();
	oa->GetProperty()->SetLighting(false);
	oa->GetProperty()->SetLineWidth(normalOutlineWidth);
	renderer()->AddActor(oa);

	auto wa = m_setting.wireframeActor();
	wa->GetProperty()->SetLighting(false);
	renderer()->AddActor(wa);

	auto ia = m_setting.indexActor();
	renderer()->AddActor2D(ia);
}

void PreProcessorStructured2dGridShapeDataItem::informGridUpdate()
{
	updateActorSetting();
}

void PreProcessorStructured2dGridShapeDataItem::updateActorSetting()
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

void PreProcessorStructured2dGridShapeDataItem::showPropertyDialog()
{
	showPropertyDialogModeless();
}

QDialog* PreProcessorStructured2dGridShapeDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = new GraphicsWindowDataItemUpdateActorSettingDialog(this, parent);
	auto widget = new GridShapeSettingEditWidget(dialog);
	dialog->setWidget(widget);

	widget->setSetting(&m_setting);
	dialog->setWindowTitle(tr("Grid Shape Setting"));

	return dialog;
}

void PreProcessorStructured2dGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(selectedOutlineWidth);
	gridDataItem()->selectedNodesController()->setVisibility(true);
	updateVisibility();
}

void PreProcessorStructured2dGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_setting.outlineActor()->GetProperty()->SetLineWidth(normalOutlineWidth);
	gridDataItem()->selectedNodesController()->setVisibility(false);
}

void PreProcessorStructured2dGridShapeDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void PreProcessorStructured2dGridShapeDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_setting.outlineActor()->SetPosition(0, 0, range.max());
	m_setting.wireframeActor()->SetPosition(0, 0, range.min());
}
