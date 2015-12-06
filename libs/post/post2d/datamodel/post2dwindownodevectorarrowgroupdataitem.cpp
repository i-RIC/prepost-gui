#include "../post2dwindowgraphicsview.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
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

const double Post2dWindowNodeVectorArrowGroupDataItem::MINLIMIT = 1.0E-6;

Post2dWindowNodeVectorArrowGroupDataItem::Setting::Setting() :
	CompositeContainer
		({&scalarValueName, &currentSolution, &color,&oldCameraScale, &scaleFactor, &regionMode,
		 &mapping, &legendMode, &lengthMode, &standardValue, &legendLength, &minimumValue, &arrowSetting}),
	scalarValueName {"scalarValueName"},
	currentSolution {"solution"},
	color {"color"},
	oldCameraScale {"oldCameraScale", 1},
	scaleFactor {"scaleFactor", 1},
	regionMode {"regionMode", StructuredGridRegion::rmFull},
	mapping {"mapping", Specific},
	legendMode {"legendMode", lmAuto},
	lengthMode {"lengthMode", lenAuto},
	standardValue {"standardValue", 1},
	legendLength {"legendLength", STANDARD_LENGTH},
	minimumValue {"minimumValue", 0.001},
	arrowSetting {}
{
	QSettings setting;
	color = setting.value("graphics/vectorcolor", QColor(Qt::black)).value<QColor>();
	scaleFactor = setting.value("graphics/vectorfactor", 1).value<double>();
}

Post2dWindowNodeVectorArrowGroupDataItem::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

Post2dWindowNodeVectorArrowGroupDataItem::Setting& Post2dWindowNodeVectorArrowGroupDataItem::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

Post2dWindowNodeVectorArrowGroupDataItem::Post2dWindowNodeVectorArrowGroupDataItem(Post2dWindowDataItem* p) :
	Post2dWindowDataItem {tr("Arrow"), QIcon(":/libs/guibase/images/iconFolder.png"), p}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	SolverDefinitionGridType* gt = cont->gridType();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(cont->data()->GetPointData())) {
		auto item = new Post2dWindowNodeVectorArrowDataItem(name, gt->solutionCaption(name), this);
		m_childItems.append(item);
	}
	setupActors();
}

Post2dWindowNodeVectorArrowGroupDataItem::~Post2dWindowNodeVectorArrowGroupDataItem()
{
	renderer()->RemoveActor(m_arrowActor);
}

class Post2dWindowNodeVectorArrowGroupDataItem::SelectSolutionCommand : public QUndoCommand
{
public:
	SelectSolutionCommand(const std::string& newsol, Post2dWindowNodeVectorArrowGroupDataItem* item) :
		QUndoCommand {Post2dWindowNodeVectorArrowGroupDataItem::tr("Arrow Physical Value Change")},
		m_newCurrentSolution (newsol),
		m_oldCurrentSolution (item->m_setting.currentSolution),
		m_item {item}
	{}
	void redo() {
		m_item->setCurrentSolution(m_newCurrentSolution);
		m_item->updateActorSettings();
	}
	void undo() {
		m_item->setCurrentSolution(m_oldCurrentSolution);
		m_item->updateActorSettings();
	}
private:
	std::string m_newCurrentSolution;
	std::string m_oldCurrentSolution;

	Post2dWindowNodeVectorArrowGroupDataItem* m_item;
};

void Post2dWindowNodeVectorArrowGroupDataItem::handleNamedItemChange(NamedGraphicWindowDataItem* item)
{
	if (item->standardItem()->checkState() != Qt::Checked) {
		pushRenderCommand(new SelectSolutionCommand("", this), this, true);
	} else {
		pushRenderCommand(new SelectSolutionCommand(item->name(), this), this, true);
	}
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
	m_hedgeHog->SetScaleFactor(m_setting.scaleFactor);

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

	m_activePoints = vtkSmartPointer<vtkPolyData>::New();
	m_hedgeHog->SetInputData(m_activePoints);
	m_warpVector->SetInputData(m_activePoints);

	renderer()->AddActor2D(m_baseArrowActor);
}

void Post2dWindowNodeVectorArrowGroupDataItem::calculateStandardValue()
{
	if (m_setting.lengthMode == lenCustom) {return;}
	QVector<double> lenVec;
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_setting.currentSolution == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	vtkDataArray* da = pd->GetArray(iRIC::toStr(m_setting.currentSolution).c_str());
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

void Post2dWindowNodeVectorArrowGroupDataItem::informGridUpdate()
{
	updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateActorSettings()
{
	m_arrowActor->VisibilityOff();
	m_legendTextActor->VisibilityOff();
	m_baseArrowActor->VisibilityOff();

	m_actorCollection->RemoveAllItems();
	m_actor2DCollection->RemoveAllItems();

	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	vtkPointSet* ps = cont->data();
	if (m_setting.currentSolution == "") {return;}
	vtkPointData* pd = ps->GetPointData();
	if (pd->GetNumberOfArrays() == 0) {return;}
	pd->SetActiveVectors(iRIC::toStr(m_setting.currentSolution).c_str());

	updateActivePoints();

	calculateStandardValue();
	updateColorSetting();
	updatePolyData();
	updateLegendData();

	m_actorCollection->AddItem(m_arrowActor);
	m_actor2DCollection->AddItem(m_legendTextActor);
	m_actor2DCollection->AddItem(m_baseArrowActor);
	updateVisibilityWithoutRendering();
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateColorSetting()
{
	Post2dWindowGridTypeDataItem* typedi = dynamic_cast<Post2dWindowGridTypeDataItem*>(parent()->parent());
	switch (Mapping(m_setting.mapping)) {
	case Specific:
		m_arrowMapper->ScalarVisibilityOff();
		m_arrowActor->GetProperty()->SetColor(m_setting.color);
		break;
	case Scalar:
		m_arrowMapper->ScalarVisibilityOn();
		LookupTableContainer* stc = typedi->lookupTable(iRIC::toStr(m_setting.scalarValueName).c_str());
		m_arrowMapper->SetScalarModeToUsePointFieldData();
		m_arrowMapper->SelectColorArray(iRIC::toStr(m_setting.scalarValueName).c_str());
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

void Post2dWindowNodeVectorArrowGroupDataItem::setCurrentSolution(const std::string& currentSol)
{
	NamedGraphicsWindowDataItemTool::checkItemWithName(currentSol, m_childItems);
	m_setting.currentSolution = currentSol.c_str();
}

void Post2dWindowNodeVectorArrowGroupDataItem::innerUpdate2Ds()
{
	vtkCamera* cam = renderer()->GetActiveCamera();
	double scale = cam->GetParallelScale();
	if (scale != m_setting.oldCameraScale) {
		updatePolyData();
		updateLegendData();
	}
	m_setting.oldCameraScale = scale;
}

void Post2dWindowNodeVectorArrowGroupDataItem::updatePolyData()
{
	PostZoneDataContainer* cont = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->dataContainer();
	if (cont == nullptr || cont->data() == nullptr) {return;}
	if (m_setting.currentSolution == "") {return;}
	updateScaleFactor();
	double height = dataModel()->graphicsView()->stdRadius(m_setting.arrowSetting.arrowSize());
	m_hedgeHog->SetScaleFactor(m_setting.scaleFactor);
	m_warpVector->SetScaleFactor(m_setting.scaleFactor);
	m_arrowSource->SetHeight(height);
	m_arrowSource->SetAngle(15);
	m_arrowSource->Modified();

	m_appendPolyData->Update();
	m_polyData->DeepCopy(m_appendPolyData->GetOutput());
	m_arrowActor->GetProperty()->SetLineWidth(m_setting.arrowSetting.lineWidth());
	m_baseArrowActor->GetProperty()->SetLineWidth(m_setting.arrowSetting.lineWidth());
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateScaleFactor()
{
	double a = 1.0 / dataModel()->graphicsView()->stdRadius(1.0);
	m_setting.scaleFactor = m_setting.legendLength / (a * m_setting.standardValue);
}

void Post2dWindowNodeVectorArrowGroupDataItem::updateLegendData()
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

	QString lenStr = QString("%1\n\n%2").arg(m_setting.currentSolution).arg(m_setting.standardValue);
	m_legendTextActor->SetInput(iRIC::toStr(lenStr).c_str());

	if (m_setting.mapping == Specific) {
		// specified color.
		m_baseArrowActor->GetProperty()->SetColor(m_setting.color);
	} else if (m_setting.mapping == Scalar) {
		// always black.
		m_baseArrowActor->GetProperty()->SetColor(0, 0, 0);
	}
}

void Post2dWindowNodeVectorArrowGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_setting.load(node);
	setCurrentSolution(m_setting.currentSolution);
}

void Post2dWindowNodeVectorArrowGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_setting.save(writer);
}

void Post2dWindowNodeVectorArrowGroupDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
