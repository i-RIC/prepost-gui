#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/graphicsmisc.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QStandardItem>
#include <QTextStream>
#include <QUndoCommand>
#include <QVector2D>

#include <vtkActor2DCollection.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkDiskSource.h>
#include <vtkGeometryFilter.h>
#include <vtkIdentityTransform.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActor.h>
#include <vtkStructuredGrid.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTriangle.h>

#include <cmath>

Post2dWindowNodeVectorArrowGroupDataItem::Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.svg"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData())) {
		auto item = new Post2dWindowNodeVectorArrowDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cont->data()->GetPointData())){
		m_colorbarTitleMap.insert(name, name.c_str());
	}

	setupActors();
}

Post2dWindowNodeVectorArrowGroupDataItem::~Post2dWindowNodeVectorArrowGroupDataItem()
{
	auto r = renderer();
	r->RemoveActor(m_arrowActor);
	r->RemoveActor2D(m_legendActors.arrowActor());
	r->RemoveActor2D(m_legendActors.nameTextActor());
	r->RemoveActor2D(m_legendActors.valueTextActor());

	m_scalarBarWidget->SetInteractor(nullptr);
}

void Post2dWindowNodeVectorArrowGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Arrow Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void Post2dWindowNodeVectorArrowGroupDataItem::setupActors()
{
	m_arrowActor = vtkSmartPointer<vtkActor>::New();
	renderer()->AddActor(m_arrowActor);
	m_arrowActor->GetProperty()->LightingOff();

	m_arrowMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_arrowActor->SetMapper(m_arrowMapper);

	m_hedgeHog = vtkSmartPointer<vtkHedgeHog>::New();
	m_hedgeHog->SetVectorModeToUseVector();
	m_hedgeHog->SetScaleFactor(m_scaleFactor);

	m_warpVector = vtkSmartPointer<vtkWarpVector>::New();

	m_arrowGlyph = vtkSmartPointer<vtkGlyph3D>::New();
	m_arrowGlyph->SetScaleModeToDataScalingOff();
	m_arrowGlyph->SetVectorModeToUseVector();
	m_arrowGlyph->SetInputConnection(m_warpVector->GetOutputPort());

	m_arrowSource = vtkSmartPointer<vtkConeSource>::New();
	m_arrowGlyph->SetSourceConnection(m_arrowSource->GetOutputPort());

	m_appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
	m_appendPolyData->AddInputConnection(m_hedgeHog->GetOutputPort());
	m_appendPolyData->AddInputConnection(m_arrowGlyph->GetOutputPort());

	m_polyData = vtkSmartPointer<vtkPolyData>::New();
	m_arrowMapper->SetInputData(m_polyData);

	m_arrowActor->VisibilityOff();

	m_legendActors.setPosition(0.75, 0.06);

	m_legendActors.nameTextActor()->VisibilityOff();
	renderer()->AddActor2D(m_legendActors.nameTextActor());

	m_legendActors.valueTextActor()->VisibilityOff();
	renderer()->AddActor2D(m_legendActors.valueTextActor());

	m_activePoints = vtkSmartPointer<vtkPolyData>::New();

	m_transformedActivePoints = vtkSmartPointer<vtkTransformFilter>::New();
	vtkSmartPointer<vtkIdentityTransform> transform = vtkSmartPointer<vtkIdentityTransform>::New();
	m_transformedActivePoints->SetTransform(transform);
	m_transformedActivePoints->SetInputData(m_activePoints);

	m_hedgeHog->SetInputConnection(m_transformedActivePoints->GetOutputPort());
	m_warpVector->SetInputConnection(m_transformedActivePoints->GetOutputPort());

	renderer()->AddActor2D(m_legendActors.arrowActor());

	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	Q_ASSERT(iren != nullptr);
	m_scalarBarWidget = vtkScalarBarWidget::New();
	iRIC::setupScalarBarProperty(m_scalarBarWidget->GetScalarBarActor());
	m_scalarBarWidget->SetInteractor(iren);
	m_scalarBarWidget->SetEnabled(0);
}

void Post2dWindowNodeVectorArrowGroupDataItem::calculateStandardValue()
{
	auto& s = setting();

	if (s.lengthMode == ArrowSettingContainer::LengthMode::Custom) {return;}

	QVector<double> lenVec;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (s.target == "") {return;}

	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(s.target).c_str());
	for (vtkIdType i = 0; i < da->GetNumberOfTuples(); ++i) {
		double* v = da->GetTuple3(i);
		QVector2D vec(*(v), *(v + 1));
		lenVec.append(vec.length());
	}
	qSort(lenVec);
	double sum = 0;
	int count = AUTO_AVERAGECOUNT;
	if (count > lenVec.count()) {count = lenVec.count();}
	for (int i = 0; i < count; ++i) {
		sum += lenVec.at(lenVec.count() - i - 1);
	}
	double average = sum / count;
	if (average == 0) {average = 1;}

	int p = 0;
	double p2 = 1;
	while (average > 10) {
		average /= 10.;
		++ p;
		p2 = 10;
	}
	while (average < 1) {
		average *= 10;
		++ p;
		p2 = 0.1;
	}
	average = static_cast<int>(average);
	for (int i = 0; i < p; ++i) {
		average *= p2;
	}
	// now average is calculated.
	s.standardValue = average;
	// minimum value is always 0.001 * standard value when auto mode.
	s.minimumValue = 0.001 * s.standardValue;
}

void Post2dWindowNodeVectorArrowGroupDataItem::setupScalarBarSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());

	auto& s = setting();

	vtkScalarBarActor* a = m_scalarBarWidget->GetScalarBarActor();
	a->SetTitle(iRIC::toStr(m_colorbarTitleMap.value(iRIC::toStr(s.colorTarget))).c_str());
	a->SetLookupTable(typedi->nodeLookupTable(s.colorTarget)->vtkObj());
	a->SetNumberOfLabels(s.scalarBarSetting.numberOfLabels);
	a->SetMaximumNumberOfColors(256);
	s.scalarBarSetting.titleTextSetting.applySetting(a->GetTitleTextProperty());
	s.scalarBarSetting.labelTextSetting.applySetting(a->GetLabelTextProperty());
	s.scalarBarSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
}

void Post2dWindowNodeVectorArrowGroupDataItem::innerUpdateZScale(double scale)
{
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	transform->Scale(1, scale, 1);
	m_transformedActivePoints->SetTransform(transform);

	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateActorSettings()
{
	m_arrowActor->VisibilityOff();
	m_legendActors.nameTextActor()->VisibilityOff();
	m_legendActors.valueTextActor()->VisibilityOff();
	m_legendActors.arrowActor()->VisibilityOff();
	m_scalarBarWidget->GetScalarBarActor()->VisibilityOff();
	m_scalarBarWidget->SetEnabled(0);

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	auto& s = setting();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	setupScalarBarSetting();  // avoid a null vtkScalarBarActor::LookupTable when using color 'By scalar value'
	if (s.target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	pd->SetActiveVectors(iRIC::toStr(s.target).c_str());

	updateActivePoints();

	calculateStandardValue();
	updateColorSetting();
	updatePolyData();
	updateLegendData();

	m_actorCollection->AddItem(m_arrowActor);
	m_actor2DCollection->AddItem(m_legendActors.nameTextActor());
	m_actor2DCollection->AddItem(m_legendActors.valueTextActor());
	m_actor2DCollection->AddItem(m_legendActors.arrowActor());
	updateVisibilityWithoutRendering();
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateColorSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());

	auto& s = setting();

	switch (s.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(s.customColor);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = typedi->nodeLookupTable(s.colorTarget);
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(iRIC::toStr(s.colorTarget).c_str());
		m_arrowMapper->SetLookupTable(stc->vtkObj());
		m_arrowMapper->UseLookupTableScalarRangeOn();
		break;
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void Post2dWindowNodeVectorArrowGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_arrowActor->SetPosition(0, 0, range.max());
}

void Post2dWindowNodeVectorArrowGroupDataItem::update()
{
	informGridUpdate();
}

std::string Post2dWindowNodeVectorArrowGroupDataItem::target() const
{
	return const_cast<Post2dWindowNodeVectorArrowGroupDataItem*> (this)->setting().target;
}

std::string Post2dWindowNodeVectorArrowGroupDataItem::colorScalar() const
{
	return const_cast<Post2dWindowNodeVectorArrowGroupDataItem*> (this)->setting().colorTarget;
}

void Post2dWindowNodeVectorArrowGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	setting().target = target.c_str();
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateVisibility(bool visible)
{
	auto& s = setting();
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_scalarBarWidget->SetEnabled(s.scalarBarSetting.visible.value() && v && (s.colorMode == ArrowSettingContainer::ColorMode::ByScalar) && s.colorTarget != "");
	Post2dWindowDataItem::updateVisibility(visible);
}

void Post2dWindowNodeVectorArrowGroupDataItem::innerUpdate2Ds()
{
	auto& s = setting();

	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != s.oldCameraScale) {
		updatePolyData();
		updateLegendData();
	}
	s.oldCameraScale = scale;
}

void Post2dWindowNodeVectorArrowGroupDataItem::updatePolyData()
{
	auto& s = setting();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (s.target == "") {return;}

	updateScaleFactor();
	double height = dataModel()->graphicsView()->stdRadius(s.arrowSize);
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
	m_arrowActor->GetProperty()->SetLineWidth(s.lineWidth);
	m_legendActors.arrowActor()->GetProperty()->SetLineWidth(s.lineWidth);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateScaleFactor()
{
	auto& s = setting();
	double a = 1.0 / dataModel()->graphicsView()->stdRadius(1.0);
	m_scaleFactor = s.legendLength / (a * s.standardValue);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateLegendData()
{
	auto& s = setting();

	double arrowLen = s.legendLength;
	m_legendActors.update(iRIC::toStr(s.target), s.legendLength, s.standardValue, s.arrowSize, 25.0);

	if (s.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		// specified color.
		m_legendActors.arrowActor()->GetProperty()->SetColor(s.customColor);
	} else if (s.colorMode == ArrowSettingContainer::ColorMode::ByScalar) {
		// always black.
		m_legendActors.arrowActor()->GetProperty()->SetColor(0, 0, 0);
	}
	s.legendTextSetting.applySetting(m_legendActors.nameTextActor()->GetTextProperty());
	s.legendTextSetting.applySetting(m_legendActors.valueTextActor()->GetTextProperty());
}

void Post2dWindowNodeVectorArrowGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}
