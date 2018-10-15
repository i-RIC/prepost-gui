#include "measureddatafiledataitem.h"
#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectordataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "measureddatavectorsettingdialog.h"
#include "private/measureddatavectorgroupdataitem_impl.h"
#include "private/measureddatavectorgroupdataitem_setsettingcommand.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/misc/targeted/targeteditemsettargetcommandtool.h>
#include <guicore/named/namedgraphicswindowdataitemtool.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

#include <QDomElement>
#include <QIcon>
#include <QSettings>
#include <QVector2D>

#include <vtkActor2DCollection.h>
#include <vtkAppendPolyData.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkConeSource.h>
#include <vtkDoubleArray.h>
#include <vtkGeometryFilter.h>
#include <vtkGlyph3D.h>
#include <vtkHedgeHog.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTriangle.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertex.h>
#include <vtkWarpVector.h>

#include <cmath>
#include <vector>

const double MeasuredDataVectorGroupDataItem::MINLIMIT = 1.0E-6;

MeasuredDataVectorGroupDataItem::Impl::Impl(MeasuredDataVectorGroupDataItem* item) :
	m_arrowActor {vtkActor::New()},
	m_arrowMapper {vtkPolyDataMapper::New()},
	m_appendPolyData {vtkAppendPolyData::New()},
	m_polyData {vtkPolyData::New()},
	m_hedgeHog {vtkHedgeHog::New()},
	m_arrowGlyph {vtkGlyph3D::New()},
	m_warpVector {vtkWarpVector::New()},
	m_activePoints {vtkPolyData::New()},
	m_arrowSource {vtkConeSource::New()},
	m_legendTextActor {vtkTextActor::New()},
	m_baseArrowActor {vtkActor2D::New()},
	m_baseArrowPolyData {vtkUnstructuredGrid::New()},
	m_item {item}
{}

MeasuredDataVectorGroupDataItem::Impl::~Impl()
{
	m_item->renderer()->RemoveActor(m_arrowActor);

	m_arrowActor->Delete();
	m_arrowMapper->Delete();

	m_appendPolyData->Delete();
	m_polyData->Delete();

	m_hedgeHog->Delete();
	m_arrowGlyph->Delete();
	m_warpVector->Delete();

	m_activePoints->Delete();
	m_arrowSource->Delete();

	m_legendTextActor->Delete();
	m_baseArrowActor->Delete();
	m_baseArrowPolyData->Delete();
}

void MeasuredDataVectorGroupDataItem::Impl::setupActors()
{
	vtkRenderer* r = m_item->renderer();
	r->AddActor(m_arrowActor);
	m_arrowActor->GetProperty()->LightingOff();

	m_arrowActor->SetMapper(m_arrowMapper);

	m_hedgeHog->SetVectorModeToUseVector();
	m_hedgeHog->SetScaleFactor(m_scaleFactor);

	m_arrowGlyph->SetScaleModeToDataScalingOff();
	m_arrowGlyph->SetVectorModeToUseVector();
	m_arrowGlyph->SetInputConnection(m_warpVector->GetOutputPort());

	m_arrowGlyph->SetSourceConnection(m_arrowSource->GetOutputPort());

	m_appendPolyData->AddInputConnection(m_hedgeHog->GetOutputPort());
	m_appendPolyData->AddInputConnection(m_arrowGlyph->GetOutputPort());

	m_arrowMapper->SetInputData(m_polyData);

	m_arrowActor->VisibilityOff();

	m_legendTextActor->SetTextScaleModeToNone();
	m_legendTextActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_legendTextActor->SetPosition(0.75, 0.02);
	vtkTextProperty* prop = m_legendTextActor->GetTextProperty();
	prop->SetColor(0, 0, 0);
	prop->SetFontFamilyToArial();
	prop->SetJustificationToCentered();
	prop->SetVerticalJustificationToBottom();

	m_legendTextActor->VisibilityOff();
	r->AddActor2D(m_legendTextActor);

	vtkSmartPointer<vtkPolyDataMapper2D> mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	vtkSmartPointer<vtkGeometryFilter> f = vtkSmartPointer<vtkGeometryFilter>::New();
	f->SetInputData(m_baseArrowPolyData);
	mapper->SetInputConnection(f->GetOutputPort());

	m_baseArrowActor->SetMapper(mapper);

	m_baseArrowActor->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	m_baseArrowActor->GetPositionCoordinate()->SetValue(.75, .02);
	m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	m_baseArrowActor->VisibilityOff();

	r->AddActor2D(m_baseArrowActor);
}

void MeasuredDataVectorGroupDataItem::Impl::updatePolyData()
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(m_item->parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {return;}
	if (m_setting.target == "") {return;}
	updateScaleFactor();
	VTKGraphicsView* view = m_item->dataModel()->graphicsView();
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);

	double height = view2->stdRadius(8);
	m_hedgeHog->SetScaleFactor(m_scaleFactor);
	m_warpVector->SetScaleFactor(m_scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
}

void MeasuredDataVectorGroupDataItem::Impl::updateLegendData()
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

	QString lenStr = QString("%1\n\n%2").arg(m_setting.target).arg(m_setting.standardValue);
	m_legendTextActor->SetInput(iRIC::toStr(lenStr).c_str());

	if (m_setting.colorMode == ArrowSettingContainer::ColorMode::Custom) {
		// specified color.
		m_baseArrowActor->GetProperty()->SetColor(m_setting.customColor);
	} else if (m_setting.colorMode == ArrowSettingContainer::ColorMode::ByScalar) {
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

void MeasuredDataVectorGroupDataItem::Impl::updateScaleFactor()
{
	VTKGraphicsView* view = m_item->dataModel()->graphicsView();
	VTK2DGraphicsView* view2 = dynamic_cast<VTK2DGraphicsView*>(view);
	double a = 1.0 / view2->stdRadius(1.0);
	m_scaleFactor = m_setting.legendLength / (a * m_setting.standardValue);
}

void MeasuredDataVectorGroupDataItem::Impl::updateColorSetting()
{
	MeasuredDataFileDataItem* fdi = dynamic_cast<MeasuredDataFileDataItem*>(m_item->parent());
	MeasuredDataPointGroupDataItem* pgdi = fdi->pointGroupDataItem();
	switch (m_setting.colorMode.value()) {
	case ArrowSettingContainer::ColorMode::Custom:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(m_setting.customColor);
		break;
	case ArrowSettingContainer::ColorMode::ByScalar:
		// not implemented yet.
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = pgdi->lookupTable(iRIC::toStr(m_setting.colorTarget));
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(iRIC::toStr(m_setting.colorTarget).c_str());
		m_arrowMapper->SetLookupTable(stc->vtkObj());
		m_arrowMapper->UseLookupTableScalarRangeOn();
		break;
	}
}

void MeasuredDataVectorGroupDataItem::Impl::calculateStandardValue()
{
	if (m_setting.lengthMode == ArrowSettingContainer::LengthMode::Custom) {return;}

	QVector<double> lenVec;
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(m_item->parent())->measuredData();
	if (md == 0 || md->vectorNames().size() == 0) {return;}
	vtkPointSet* ps = md->pointData();
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
	m_setting.minimumValue = 0.001 * m_setting.standardValue;
}

vtkPointSet* MeasuredDataVectorGroupDataItem::Impl::getPointSet()
{
	m_activePoints->Reset();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(m_item->parent())->measuredData();
	vtkPointSet* ps = md->polyData();

	vtkDoubleArray* vectorArray = vtkDoubleArray::SafeDownCast(ps->GetPointData()->GetArray(iRIC::toStr(m_setting.target).c_str()));
	std::vector<vtkIdType> points;
	double min = m_setting.minimumValue;
	double minlimitsqr = min * min;
	for (vtkIdType i = 0; i < ps->GetNumberOfPoints(); ++i) {
		bool active = true;
		double val = 0;
		for (int j = 0; j < vectorArray->GetNumberOfComponents(); ++j) {
			double tmpval = vectorArray->GetComponent(i, j);
			val += tmpval * tmpval;
		}
		if (val < minlimitsqr) {
			active = false;
		}
		if (active) {
			points.push_back(i);
		}
	}

	vtkPointData* inPD = ps->GetPointData();
	vtkPointData* outPD = m_activePoints->GetPointData();
	vtkPoints* inPoints = ps->GetPoints();
	vtkSmartPointer<vtkPoints> outPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();

	outPoints->SetDataTypeToDouble();
	outPD->CopyAllocate(inPD, points.size());
	int newId = 0;
	for (auto it = points.begin(); it != points.end(); ++it) {
		vtkIdType pointid = *it;
		outPoints->InsertNextPoint(inPoints->GetPoint(pointid));
		verts->InsertNextCell(1, &pointid);
		outPD->CopyData(inPD, pointid, newId);
		++ newId;
	}

	m_activePoints->SetPoints(outPoints);
	m_activePoints->SetVerts(verts);
	m_activePoints->Modified();
	return m_activePoints;
}

// public interfaces

MeasuredDataVectorGroupDataItem::MeasuredDataVectorGroupDataItem(GraphicsWindowDataItem* p) :
	GraphicsWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();

	for (std::string name : md->vectorNames()) {
		auto item = new MeasuredDataVectorDataItem(name, name.c_str(), this);
		m_childItems.push_back(item);
	}

	impl->setupActors();

	if (md->vectorNames().size() > 0) {
		auto name = md->vectorNames().at(0);
		setTarget(name);
	}
}

MeasuredDataVectorGroupDataItem::~MeasuredDataVectorGroupDataItem()
{
	delete impl;
}

void MeasuredDataVectorGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_setting.load(node);
	setTarget(iRIC::toStr(impl->m_setting.target));
}

void MeasuredDataVectorGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
}

std::string MeasuredDataVectorGroupDataItem::target() const
{
	return impl->m_setting.target;
}

void MeasuredDataVectorGroupDataItem::setTarget(const std::string &target)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(target, m_childItems);
	impl->m_setting.target = target.c_str();
	updateActorSettings();
}

void MeasuredDataVectorGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (m_isCommandExecuting) {return;}

	auto cmd = TargetedItemSetTargetCommandTool::buildFromNamedItem(item, this, tr("Arrow Physical Value Change"));
	pushRenderCommand(cmd, this, true);
}

void MeasuredDataVectorGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void MeasuredDataVectorGroupDataItem::updateActorSettings()
{
	impl->m_arrowActor->VisibilityOff();
	impl->m_legendTextActor->VisibilityOff();
	impl->m_baseArrowActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == 0 || md->pointData() == 0) {return;}
	if (impl->m_setting.target == "") {return;}
	vtkPointSet* ps = impl->getPointSet();
	if (impl->m_setting.target == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}

	pd->SetActiveVectors(iRIC::toStr(impl->m_setting.target).c_str());
	impl->m_hedgeHog->SetInputData(ps);
	impl->m_warpVector->SetInputData(ps);

	impl->calculateStandardValue();
	impl->updateColorSetting();
	impl->updatePolyData();
	impl->updateLegendData();

	m_actorCollection->AddItem(impl->m_arrowActor);
	m_actor2DCollection->AddItem(impl->m_legendTextActor);
	m_actor2DCollection->AddItem(impl->m_baseArrowActor);
	updateVisibilityWithoutRendering();
}

void MeasuredDataVectorGroupDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(2);
}

void MeasuredDataVectorGroupDataItem::assignActorZValues(const ZDepthRange& range)
{
	impl->m_arrowActor->SetPosition(0, 0, range.max());
}

void MeasuredDataVectorGroupDataItem::update()
{
	informGridUpdate();
}

void MeasuredDataVectorGroupDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != impl->m_setting.oldCameraScale) {
		impl->updatePolyData();
		impl->updateLegendData();
	}
	impl->m_setting.oldCameraScale = scale;
}

QDialog* MeasuredDataVectorGroupDataItem::propertyDialog(QWidget* p)
{
	MeasuredData* md = dynamic_cast<MeasuredDataFileDataItem*>(parent())->measuredData();
	if (md == nullptr || md->pointData() == nullptr) {
		return 0;
	}
	if (md->vectorNames().size() == 0) {
		return 0;
	}
	MeasuredDataVectorSettingDialog* dialog = new MeasuredDataVectorSettingDialog(p);
	dialog->setData(md);
	dialog->setSetting(impl->m_setting);

	return dialog;
}

void MeasuredDataVectorGroupDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	MeasuredDataVectorSettingDialog* dialog = dynamic_cast<MeasuredDataVectorSettingDialog*>(propDialog);
	pushRenderCommand(new SetSettingCommand(dialog->setting(), this), this, true);
}

void MeasuredDataVectorGroupDataItem::doApplyOffset(double /*x*/, double /*y*/)
{
	this->updateActorSettings();
}
