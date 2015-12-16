#include "../post3dwindowgraphicsview.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowparticlesvectorgroupdataitem.h"
#include "post3dwindowparticlesvectordataitem.h"
#include "post3dwindowzonedataitem.h"
#include "private/post3dwindowparticlesvectorgroupdataitem_setsettingcommand.h"

#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <postbase/particle/postparticlevectorpropertydialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QFile>
#include <QMenu>
#include <QMouseEvent>
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
#include <vtkTransform.h>
#include <vtkLine.h>
#include <vtkLineSource.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>

#include <cmath>

Post3dWindowParticlesVectorGroupDataItem::Post3dWindowParticlesVectorGroupDataItem(Post3dWindowDataItem* p) :
	Post3dWindowDataItem(tr("Vector"), QIcon(":/libs/guibase/images/iconFolder.png"), p),
	m_scaleFactor {1}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->particleData()->GetPointData())) {
		auto item = new Post3dWindowParticlesVectorDataItem(name, gt->solutionCaption(name), this);
		m_childItems.push_back(item);
	}
	setupActors();
}

Post3dWindowParticlesVectorGroupDataItem::~Post3dWindowParticlesVectorGroupDataItem()
{
	renderer()->RemoveActor(m_arrowActor);
}

void Post3dWindowParticlesVectorGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Vector Attribute Select Change"));
	pushRenderCommand(cmd, this, true);
}

void Post3dWindowParticlesVectorGroupDataItem::setupActors()
{
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}

	updateScaleFactor();

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

	m_transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
	m_transformFilter->SetInputData(cont->particleData());
	vtkSmartPointer<vtkIdentityTransform> transform = vtkSmartPointer<vtkIdentityTransform>::New();
	m_transformFilter->SetTransform(transform);

	m_arrowActor->VisibilityOff();

	m_legendTextActor = vtkSmartPointer<vtkTextActor>::New();
	m_legendTextActor->SetTextScaleModeToNone();
	m_legendTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_legendTextActor->SetPosition(0.75, 0.02);
	vtkTextProperty* prop = m_legendTextActor->GetTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontFamilyToArial();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToBottom();

	m_legendTextActor->VisibilityOff();
	renderer()->AddActor2D(m_legendTextActor);

	m_baseArrowPolyData = vtkSmartPointer<vtkUnstructuredGrid>::New();

	vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	vtkSmartPointer<vtkGeometryFilter> f = vtkSmartPointer<vtkGeometryFilter>::New();
	f->SetInputData(m_baseArrowPolyData);
	mapper->SetInputConnection(f->GetOutputPort());

	m_baseArrowActor = vtkSmartPointer<vtkActor2D>::New();
	m_baseArrowActor->SetMapper(mapper);

	m_baseArrowActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_baseArrowActor->GetPositionCoordinate()->SetValue(.75, .02);
	m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	m_baseArrowActor->VisibilityOff();

	m_arrowMask = vtkSmartPointer<vtkMaskPoints>::New();
	m_arrowMask->SetInputConnection(m_transformFilter->GetOutputPort());

	m_hedgeHog->SetInputConnection(m_arrowMask->GetOutputPort());
	m_warpVector->SetInputConnection(m_arrowMask->GetOutputPort());

	renderer()->AddActor2D(m_baseArrowActor);
}

void Post3dWindowParticlesVectorGroupDataItem::calculateStandardValue()
{
	if (m_setting.lengthMode == ArrowSettingContainer::LengthMode::Custom) {return;}
	QVector<double> lenVec;
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}
	vtkPointSet* ps = cont->particleData();
	if (m_setting.target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(m_setting.target).c_str());
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
	m_setting.standardValue = average;
	// minimum value is always 0.001 * standard value when auto mode.
	m_setting.minimumValue = average * 0.001;
}

void Post3dWindowParticlesVectorGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}


std::string Post3dWindowParticlesVectorGroupDataItem::target() const
{
	return m_setting.target;

}
void Post3dWindowParticlesVectorGroupDataItem::setTarget(const std::string& target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	m_setting.target = target.c_str();
	updateActorSettings();
}

void Post3dWindowParticlesVectorGroupDataItem::updateActorSettings()
{
	m_arrowActor->VisibilityOff();
	m_legendTextActor->VisibilityOff();
	m_baseArrowActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}
	vtkPointSet* ps = cont->particleData();
	if (m_setting.target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	pd->SetActiveVectors(iRIC::toStr(m_setting.target).c_str());

	calculateStandardValue();
	updateMaskSetting();
	updateColorSetting();
	updatePolyData();
	updateLegendData();

	m_actorCollection->AddItem(m_arrowActor);
	m_actor2DCollection->AddItem(m_legendTextActor);
	m_actor2DCollection->AddItem(m_baseArrowActor);
	updateVisibilityWithoutRendering();
}

void Post3dWindowParticlesVectorGroupDataItem::updateColorSetting()
{
	Post3dWindowGridTypeDataItem* typedi = dynamic_cast<Post3dWindowGridTypeDataItem*>(parent()->parent()->parent());
	switch (m_setting.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(m_setting.customColor);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = typedi->particleLookupTable(iRIC::toStr(m_setting.colorTarget).c_str());
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(iRIC::toStr(m_setting.colorTarget).c_str());
		m_arrowMapper->SetLookupTable(stc->vtkObj());
		m_arrowMapper->UseLookupTableScalarRangeOn();
		break;
	}
}

void Post3dWindowParticlesVectorGroupDataItem::updateMaskSetting()
{
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	vtkPolyData* data = cont->particleData();
	switch (m_setting.samplingMode.value()) {
	case ArrowSettingContainer::SamplingMode::All:
		m_arrowMask->SetOnRatio(1);
		m_arrowMask->RandomModeOff();
		m_arrowMask->SetMaximumNumberOfPoints(data->GetNumberOfPoints());
		break;
	case ArrowSettingContainer::SamplingMode::Rate:
		m_arrowMask->SetOnRatio(m_setting.samplingRate);
		m_arrowMask->SetMaximumNumberOfPoints(data->GetNumberOfPoints());
		break;
	case ArrowSettingContainer::SamplingMode::Number:
		m_arrowMask->RandomModeOn();
		m_arrowMask->SetMaximumNumberOfPoints(m_setting.samplingNumber);
		break;
	}
}

void Post3dWindowParticlesVectorGroupDataItem::update()
{
	informGridUpdate();
}

void Post3dWindowParticlesVectorGroupDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != m_setting.oldCameraScale) {
		updatePolyData();
		updateLegendData();
	}
	m_setting.oldCameraScale = scale;
}

void Post3dWindowParticlesVectorGroupDataItem::updatePolyData()
{
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	if (cont == nullptr || cont->particleData() == nullptr) {return;}
	if (m_setting.target == "") {return;}
	updateScaleFactor();
	double height = dataModel()->graphicsView()->stdDistance(m_setting.arrowSize);
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
	m_arrowActor->GetProperty()->SetLineWidth(m_setting.lineWidth);
	m_baseArrowActor->GetProperty()->SetLineWidth(m_setting.lineWidth);
}

void Post3dWindowParticlesVectorGroupDataItem::updateScaleFactor()
{
	const auto& s = m_setting;
	double a = 1.0 / dataModel()->graphicsView()->stdDistance(1.0);
	m_scaleFactor = s.legendLength / (a * s.standardValue);
}

void Post3dWindowParticlesVectorGroupDataItem::updateLegendData()
{
	double vectorOffset = 18;
	double arrowLen = m_setting.legendLength;
	m_baseArrowPolyData->Initialize();
	m_baseArrowPolyData->Allocate(3);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	m_baseArrowPolyData->SetPoints(points);
	// add line
	points->InsertNextPoint(- arrowLen * .5, vectorOffset, 0);
	points->InsertNextPoint(arrowLen * .5, vectorOffset, 0);
	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, 0);
	line->GetPointIds()->SetId(1, 1);
	m_baseArrowPolyData->InsertNextCell(line->GetCellType(), line->GetPointIds());

	// add triangle
	points->InsertNextPoint(arrowLen * .5 - 8, vectorOffset + 3, 0);
	points->InsertNextPoint(arrowLen * .5 - 8, vectorOffset - 3, 0);
	vtkSmartPointer<vtkTriangle> tri = vtkSmartPointer<vtkTriangle>::New();
	tri->GetPointIds()->SetId(0, 1);
	tri->GetPointIds()->SetId(1, 2);
	tri->GetPointIds()->SetId(2, 3);
	m_baseArrowPolyData->InsertNextCell(tri->GetCellType(), tri->GetPointIds());

	const auto& s = m_setting;
	QString lenStr = QString("%1\n\n%2").arg(s.target).arg(s.standardValue);
	m_legendTextActor->SetInput(iRIC::toStr(lenStr).c_str());

	if (s.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		// specified color.
		const QColor& cColor = s.customColor;
		m_baseArrowActor->GetProperty()->SetColor(cColor.red() / 255., cColor.green() / 255., cColor.blue() / 255.);
	} else if (s.colorMode == ArrowSettingContainer::ColorMode::ByScalar) {
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

void Post3dWindowParticlesVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{

	m_setting.load(node);
	updateScaleFactor();

	setTarget(m_setting.target);
}

void Post3dWindowParticlesVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post3dWindowParticlesVectorGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post3dWindowParticlesVectorGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post3dWindowParticlesVectorGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

QDialog* Post3dWindowParticlesVectorGroupDataItem::propertyDialog(QWidget* p)
{
	PostParticleVectorPropertyDialog* dialog = new PostParticleVectorPropertyDialog(p);
	PostZoneDataContainer* cont = dynamic_cast<Post3dWindowZoneDataItem*>(parent()->parent())->dataContainer();
	dialog->setZoneData(cont);

	dialog->setSetting(m_setting);

	return dialog;
}

void Post3dWindowParticlesVectorGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	PostParticleVectorPropertyDialog* dialog = dynamic_cast<PostParticleVectorPropertyDialog*> (propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void Post3dWindowParticlesVectorGroupDataItem::innerUpdateZScale(double zscale)
{
	auto t = vtkSmartPointer<vtkTransform>::New();
	t->Scale(1, 1, zscale);
	m_transformFilter->SetTransform(t);
	updatePolyData();
	updateLegendData();
}
