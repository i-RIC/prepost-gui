#include "post2dbirdeyewindowgridshapedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/widget/gridshapeeditdialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QIcon>
#include <QUndoCommand>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkMapperCollection.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkStructuredGrid.h>

#define LABEL "label2d"

Post2dBirdEyeWindowGridShapeDataItem::Post2dBirdEyeWindowGridShapeDataItem(Post2dBirdEyeWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {tr("Grid shape"), QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent)->dataContainer();
	if (dynamic_cast<vtkStructuredGrid*>(cont->data()) == nullptr) {
		m_setting.shape = GridShapeEditDialog::Wireframe;
	}

	setupActors();
}

Post2dBirdEyeWindowGridShapeDataItem::~Post2dBirdEyeWindowGridShapeDataItem()
{
	renderer()->RemoveActor(m_outlineActor);
	renderer()->RemoveActor(m_wireframeActor);
	renderer()->RemoveActor2D(m_indexActor);
}

void Post2dBirdEyeWindowGridShapeDataItem::setupActors()
{
	m_warp = vtkSmartPointer<vtkWarpScalar>::New();
	m_warp->SetUseNormal(1);
	m_warp->SetNormal(0, 0, 1);
	m_warp->SetScaleFactor(1);

	m_labelWarp = vtkSmartPointer<vtkWarpScalar>::New();
	m_labelWarp->SetUseNormal(1);
	m_labelWarp->SetNormal(0, 0, 1);
	m_labelWarp->SetScaleFactor(1);

	m_outlineActor = vtkSmartPointer<vtkActor>::New();
	// no lighting is needed.
	m_outlineActor->GetProperty()->SetLighting(false);
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (dynamic_cast<vtkStructuredGrid*> (cont->data()) != nullptr){
		renderer()->AddActor(m_outlineActor);
	}
	// build mapper too.
	m_outlineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_outlineActor->SetMapper(m_outlineMapper);

	// setup grid outline filter.
	m_outlineFilter = vtkSmartPointer<vtkStructuredGridOutlineFilter>::New();
	m_outlineMapper->SetInputConnection(m_outlineFilter->GetOutputPort());

	m_wireframeActor = vtkSmartPointer<vtkLODActor>::New();
	// Draw 5000 points from grid vertices.
	m_wireframeActor->SetNumberOfCloudPoints(5000);
	// Make the point size a little big.
	m_wireframeActor->GetProperty()->SetPointSize(2);
	m_wireframeActor->GetProperty()->SetLighting(false);
	renderer()->AddActor(m_wireframeActor);

	m_wireframeMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_wireframeMapper->SetScalarVisibility(false);
	m_wireframeActor->SetMapper(m_wireframeMapper);
	m_wireframeActor->GetProperty()->SetRepresentationToWireframe();

	m_indexActor = vtkSmartPointer<vtkActor2D>::New();
	renderer()->AddActor2D(m_indexActor);

	m_indexTransform = vtkSmartPointer<vtkTransform>::New();

	m_indexTransformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	m_indexTransformFilter->SetTransform(m_indexTransform);

	m_indexMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
	m_indexMapper->SetInputConnection(m_indexTransformFilter->GetOutputPort());
	m_indexMapper->SetLabelModeToLabelFieldData();
	m_indexMapper->SetFieldDataName(iRIC::toStr(PostZoneDataContainer::labelName).c_str());
	vtkTextProperty* prop = m_indexMapper->GetLabelTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontSize(12);
	prop->BoldOff();
	prop->ItalicOff();
	prop->ShadowOff();
	prop->SetJustificationToLeft();
	prop->SetVerticalJustificationToCentered();

	m_indexActor->SetMapper(m_indexMapper);
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::update()
{
	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::updateActorSettings()
{
	m_outlineActor->VisibilityOff();
	m_wireframeActor->VisibilityOff();
	m_indexActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr) {return;}
	if (cont->data() == nullptr) {return;}

	vtkPointSet* ps = cont->data();
	vtkPointSet* labeldata = cont->labelData();
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*>(ps);

	m_warp->SetInputData(ps);
	m_warp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());
	m_labelWarp->SetInputData(labeldata);
	m_labelWarp->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, iRIC::toStr(cont->elevationName()).c_str());

	vtkSmartPointer<vtkStructuredGrid> tmpgrid;
	switch (GridShapeEditDialog::Shape(m_setting.shape)) {
	case GridShapeEditDialog::Outline:
		m_outlineFilter->SetInputConnection(m_warp->GetOutputPort());
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_outlineActor);
		break;
	case GridShapeEditDialog::Wireframe:
		m_outlineFilter->SetInputConnection(m_warp->GetOutputPort());
		m_wireframeMapper->SetInputConnection(m_warp->GetOutputPort());
		m_wireframeActor->GetLODMappers()->RemoveAllItems();
		tmpgrid = grid;
		m_outlineActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_outlineActor);
		m_wireframeActor->GetProperty()->SetColor(m_setting.color);
		m_actorCollection->AddItem(m_wireframeActor);
		break;
	}
	if (m_setting.indexVisible) {
		m_labelWarp->Update();
		vtkPointSet* ps = m_labelWarp->GetOutput();
		vtkSmartPointer<vtkUnstructuredGrid> tmpGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
		tmpGrid->DeepCopy(ps);

		vtkAbstractArray* labelArray = labeldata->GetPointData()->GetAbstractArray(iRIC::toStr(PostZoneDataContainer::labelName).c_str());
		tmpGrid->GetPointData()->AddArray(labelArray);

		m_indexTransformFilter->SetInputData(tmpGrid);
		m_indexTransformFilter->Update();
		m_indexMapper->GetLabelTextProperty()->SetColor(m_setting.indexColor);
		m_actor2DCollection->AddItem(m_indexActor);
	}
	updateVisibilityWithoutRendering();
}

void Post2dBirdEyeWindowGridShapeDataItem::doLoadFromProjectMainFile(const QDomNode& shapeNode)
{
	m_setting.load(shapeNode);

	updateActorSettings();
}

void Post2dBirdEyeWindowGridShapeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dBirdEyeWindowGridShapeDataItem::handleStandardItemDoubleClicked()
{
	showPropertyDialog();
}

QDialog* Post2dBirdEyeWindowGridShapeDataItem::propertyDialog(QWidget* p)
{
	GridShapeEditDialog* dialog = new GridShapeEditDialog(p);
	dialog->setSetting(m_setting);

	PostZoneDataContainer* cont = dynamic_cast<Post2dBirdEyeWindowZoneDataItem*>(parent())->dataContainer();
	if (dynamic_cast<vtkStructuredGrid*> (cont->data()) == nullptr) {
		dialog->hideShape();
	}
	return dialog;
}

class Post2dBirdEyeWindowGridShapeDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const GridShapeEditDialog::Setting& setting, Post2dBirdEyeWindowGridShapeDataItem* item) :
		QUndoCommand {QObject::tr("Update Grid Shape Setting")},
		m_newSetting {setting},
		m_oldSetting {item->m_setting},
		m_oldEnabled {item->isEnabled()},
		m_item {item}
	{}
	void redo() {
		m_item->setEnabled(true);
		m_item->m_setting = m_newSetting;
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->setEnabled(m_oldEnabled);
		m_item->m_setting = m_oldSetting;
		m_item->updateActorSettings();
	}
private:
	GridShapeEditDialog::Setting m_newSetting;

	GridShapeEditDialog::Setting m_oldSetting;
	bool m_oldEnabled;

	Post2dBirdEyeWindowGridShapeDataItem* m_item;
};

void Post2dBirdEyeWindowGridShapeDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	GridShapeEditDialog* dialog = dynamic_cast<GridShapeEditDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void Post2dBirdEyeWindowGridShapeDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(selectedOutlineWidth);
	updateVisibility();
}

void Post2dBirdEyeWindowGridShapeDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_outlineActor->GetProperty()->SetLineWidth(normalOutlineWidth);
}

void Post2dBirdEyeWindowGridShapeDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dBirdEyeWindowGridShapeDataItem::innerUpdateZScale(double scale)
{
	m_outlineActor->SetScale(1, 1, scale);
	m_wireframeActor->SetScale(1, 1, scale);
	m_indexTransform->Identity();
	m_indexTransform->Scale(1, 1, scale);
}
