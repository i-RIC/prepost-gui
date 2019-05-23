#include "gridcreatingconditionlaplace_impl.h"
#include "gridcreatingconditionlaplace_banklinesbuilder.h"
#include "gridcreatingconditionlaplace_buildsubgridpointsthread.h"
#include "gridcreatingconditionlaplace_centerlinefinishdefiningcommand.h"
#include "gridcreatingconditionlaplace_centerlinestartdefinitioncommand.h"
#include "gridcreatingconditionlaplace_centerlineupdatelabelsandsplinecommand.h"
#include "gridcreatingconditionlaplace_commonratiooptimizethread.h"
#include "gridcreatingconditionlaplace_ctrlpointmovecommand.h"
#include "gridcreatingconditionlaplace_deploysettingcommand.h"
#include "gridcreatingconditionlaplace_divisionsettingcommand.h"
#include "gridcreatingconditionlaplace_edgeeditcoordinatescommand.h"
#include "gridcreatingconditionlaplace_edgesetinterpolationmodecommand.h"
#include "gridcreatingconditionlaplace_newedgestartdefinitioncommand.h"
#include "gridcreatingconditionlaplace_polylineaddvertexcommand.h"
#include "gridcreatingconditionlaplace_polylinedefinenewvertexcommand.h"
#include "gridcreatingconditionlaplace_polylineeditcoordinatescommand.h"
#include "gridcreatingconditionlaplace_polylinemovevertexcommand.h"
#include "gridcreatingconditionlaplace_polylineremovevertexcommand.h"
#include "gridcreatingconditionlaplace_subregiondeploysettingcommand.h"
#include "gridcreatingconditionlaplace_updateedgelinesforedgeselectioncommand.h"
#include "gridcreatingconditionlaplace_wholeregiondivisionsettingcommand.h"
#include "../gridcreatingconditionlaplacebuildbanklinesdialog.h"

#include <guibase/vtktool/vtkparametricsplineutil.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <misc/anms.h>
#include <misc/iricundostack.h>
#include <misc/linearinterpolator.h>
#include <misc/mathsupport.h>
#include <misc/splineinterpolator.h>

#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>

#include <QAction>
#include <QMenu>
#include <QMessageBox>

#include <algorithm>

namespace {

const int LINEWIDTH_WIDE = 2;
const int LINEWIDTH_NARROW = 1;
const int POINTSIZE = 5;
const int POINTSIZE_DIVPREVIEW = 3;
const int SPLINE_FACTOR = 10;
const int SUBDIVNUM = 20;
const int FONTSIZE = 17;

const double EPSILON = 1.0E-6;

struct LinePair
{
	std::vector<QPointF> line1;
	std::vector<QPointF> line2;
};

void clearPolyline(PolyLineController* line, vtkActorCollection* col, vtkRenderer* r)
{
	col->RemoveItem(line->pointsActor());
	col->RemoveItem(line->linesActor());
	r->RemoveActor(line->pointsActor());
	r->RemoveActor(line->linesActor());
	delete line;
}

void clearPolylines(std::vector<PolyLineController*>* lines, vtkActorCollection* col, vtkRenderer* r)
{
	for (auto l : *lines) {
		clearPolyline(l, col, r);
	}
	lines->clear();
}

void clearPolygon(PolygonController* polygon, vtkActorCollection* col, vtkRenderer* r)
{
	col->RemoveItem(polygon->paintActor());
	r->RemoveActor(polygon->paintActor());
	delete polygon;
}

void clearPolygons(std::vector<PolygonController*>* polygons, vtkActorCollection* col, vtkRenderer* r)
{
	for (auto p : *polygons) {
		clearPolygon(p, col, r);
	}
	polygons->clear();
}

LinePair divideLine(std::vector<QPointF>& line, int edgeId)
{
	LinePair ret;
	for (int i = 0; i < edgeId; ++i) {
		ret.line1.push_back(line.at(i));
	}
	for (int i = edgeId; i < line.size(); ++i) {
		ret.line2.push_back(line.at(i));
	}
	return ret;
}

void setupEdgeController(PolyLineController* line, const ZDepthRange& zRange)
{
	line->pointsActor()->SetPosition(0, 0, zRange.max());
	line->pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
}

void setupEdgeControllerForEdgeSelection(PolyLineController* line, const ZDepthRange& zRange)
{
	line->pointsActor()->SetPosition(0, 0, zRange.max());
	line->pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
	line->linesActor()->SetPosition(0, 0, (zRange.min() + zRange.max()) * 0.5);
	line->linesActor()->GetProperty()->SetLineWidth(LINEWIDTH_NARROW);
}

void setupEdgeControllerForDivisionPreview(PolyLineController* line, const ZDepthRange& zRange)
{
	line->pointsActor()->SetPosition(0, 0, zRange.max());
	line->pointsActor()->GetProperty()->SetPointSize(POINTSIZE_DIVPREVIEW);
	line->pointsActor()->GetProperty()->SetColor(0.3, 0.3, 0.3);
}

void setupSubRegionPolygon(PolygonController* polygon, const ZDepthRange& zRange)
{
	polygon->paintActor()->SetPosition(0, 0, zRange.min());
	polygon->paintActor()->GetProperty()->SetColor(0.5, 0.5, 0.5);
	polygon->paintActor()->GetProperty()->SetOpacity(0.7);
}

bool getPerpendicularLineOfLeg(const QPointF& p, PolyLineController& l, QPointF* leg)
{
	std::map<double, QPointF> points;

	auto polyline = l.polyLine();
	for (int i = 0; i < polyline.size() - 1; ++i) {
		auto p1 = polyline.at(i);
		auto p2 = polyline.at(i + 1);
		QPointF leg;
		double r = iRIC::perpendicularLineOfLeg(p1, p2, p, &leg);
		if (r < -EPSILON || r > 1 + EPSILON) {continue;}
		double dx = (leg.x() - p.x());
		double dy = (leg.y() - p.y());
		points.insert({dx * dx + dy * dy, leg});
	}
	if (points.size() == 0) {return false;}

	*leg = points.begin()->second;
	return true;
}

bool getIntersection(const QPointF& p1, const QPointF& p2, const std::vector<QPointF>& polyLine, int* edgeId, QPointF* intersection)
{
	for (int i = 0; i < polyLine.size() - 1; ++i) {
		auto q1 = polyLine.at(i);
		auto q2 = polyLine.at(i + 1);
		double r, s;
		bool ok = iRIC::intersectionPoint(p1, p2, q1, q2, intersection, &r, &s);
		if (! ok) {return false;}
		if (r < 0 || r > 1) {continue;}
		if (s < 0 || s > 1) {continue;}

		*edgeId = i;
		return true;
	}
	return false;
}

std::vector<QPointF> splineGenerator(const std::vector<QPointF>& points)
{
	std::vector<QPointF> copyedPoints = points;
	// make sure that the last two points are diffrent points
	auto p1 = copyedPoints.at(copyedPoints.size() - 1);
	auto p2 = copyedPoints.at(copyedPoints.size() - 2);
	if (p1 == p2) {copyedPoints.pop_back();}
	std::vector<QPointF> ret;
	if (copyedPoints.size() < 2) {return ret;}
	SplineInterpolator interpolator(copyedPoints);
	for (int i = 0; i < copyedPoints.size() - 1; ++i) {
		for (int j = 0; j < SUBDIVNUM; ++ j) {
			QPointF p = interpolator.interpolate(i, j / static_cast<double> (SUBDIVNUM));
			ret.push_back(p);
		}
	}
	ret.push_back(copyedPoints.back());

	return ret;
}

std::vector<QPointF> linearGenerator(const std::vector<QPointF>& points)
{
	std::vector<QPointF> ret;
	for (int i = 0; i < points.size() - 1; ++i) {
		LinearInterpolator2D1 interpolator(points.at(i), points.at(i + 1));
		for (int j = 0; j < SUBDIVNUM; ++j) {
			QPointF p = interpolator.interpolate(j / static_cast<double> (SUBDIVNUM));
			ret.push_back(p);
		}
	}
	ret.push_back(points.back());
	return ret;
}

void addLinePoints(const std::vector<QPointF>& line, std::vector<QPointF>* ret, std::vector<QPointF> (*pointsGenerator)(const std::vector<QPointF>&))
{
	std::vector<QPointF> newPoints = (*pointsGenerator)(line);
	if (ret->size() > 0) {
		newPoints.erase(newPoints.begin());
	}
	for (auto p : newPoints) {
		ret->push_back(p);
	}
}

QPointF getPointForLength(double length, const std::vector<double>& lengths, const std::vector<QPointF>& line)
{
	auto it = std::lower_bound(lengths.begin(), lengths.end(), length);
	auto idx = it - lengths.begin();

	if (idx == 0) {
		return line.at(0);
	} else if (idx == lengths.size()) {
		return line.back();
	}

	double l2 = lengths.at(idx);
	double l1 = lengths.at(idx - 1);
	double r = (length - l1) / (l2 - l1);

	auto p2 = line.at(idx);
	auto p1 = line.at(idx - 1);

	return p1 * (1 - r) + p2 * r;
}

void setGridPoint(vtkPoints* points, const QPointF& p, int i, int j, int imax)
{
	int idx = i + j * imax;
	points->SetPoint(idx, p.x(), p.y(), 0);
}

void setupLabelActor(vtkLabel2DActor* actor)
{
	actor->setLabelPosition(vtkLabel2DActor::lpMiddleRight);
	auto prop = actor->labelTextProperty();
	prop->SetFontSize(FONTSIZE);
	prop->BoldOn();
}

std::vector<double> setupPoissonPotential(const std::vector<double>& lengths)
{
	std::vector<double> ret;
	ret.push_back(0);
	for (int i = 1; i < lengths.size() - 1; ++i) {
		auto l1 = lengths.at(i) - lengths.at(i - 1);
		auto l2 = lengths.at(i + 1) - lengths.at(i);
		double ratio = l2 / l1;

		ret.push_back(ratio / (ratio + 1));
	}
	ret.push_back(0);
	return ret;
}

std::vector<double> setupRatios(const std::vector<double>& lengths)
{
	std::vector<double> ret;
	ret.push_back(0);
	for (int i = 1; i < lengths.size() - 1; ++i) {
		auto l1 = lengths.at(i) - lengths.at(i - 1);
		auto l2 = lengths.at(i + 1) - lengths.at(i);
		double ratio = l2 / l1;

		ret.push_back(1 / (ratio + 1));
	}
	ret.push_back(0);
	return ret;
}

class TmpGrid {
public:
	TmpGrid(int isize, int jsize) :
		m_isize {isize},
		m_jsize {jsize}
	{
		m_x.assign(isize * jsize, 0);
		m_y.assign(isize * jsize, 0);
	}
	double& x(int i, int j) {return m_x[i + j * m_isize];}
	double& y(int i, int j) {return m_y[i + j * m_isize];}

private:
	int m_isize;
	int m_jsize;

	std::vector<double> m_x;
	std::vector<double> m_y;
};

class CROptimizeParams {
public:
	std::vector<double> lengths;
	std::vector<int> divNums;
	std::vector<GridCreatingConditionLaplace::DivisionMode> divModes;
	std::vector<double> commonRatios;
};

} // namespace

GridCreatingConditionLaplace::Impl::Impl(GridCreatingConditionLaplace* cond) :
	m_editMode {EditMode::CenterLineOnly},
	m_centerLineOnlyMouseEventMode {CenterLineOnlyMouseEventMode::Normal},
	m_regionDefinedMouseEventMode {RegionDefinedMouseEventMode::Normal},
	m_centerLineIndex {-1},
	m_previewGrid {nullptr},
	m_previewGridMapper {vtkDataSetMapper::New()},
	m_previewGridActor {vtkActor::New()},
	m_optimizeThread {new CommonRatioOptimizeThread{this}},
	m_buildThread {new BuildSubGridPointsThread{this}},
	m_selectedSectionEdgeType {EdgeType::None},
	m_selectedSectionId {-1},
	m_selectedSubRegionId {-1},
	m_hoveredSectionEdgeType {EdgeType::None},
	m_hoveredSectionId {-1},
	m_hoveredEdgeId {-1},
	m_hoveredVertexId {-1},
	m_newEdgeType {EdgeType::None},
	m_newEdgeCtrlPointId {-1},
	m_newEdgeStartEdgeId {-1},
	m_newEdgeEndEdgeId {-1},
	m_newEdgeFromZero {false},
	m_itemSelected {false},
	m_manualDivSetting {false},
	m_previousLeftBankDistance {10},
	m_previousRightBankDistance {10},
	m_addCursorPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_removeCursorPixmap {":/libs/guibase/images/cursorRemove.png"},
	m_addCursor {m_addCursorPixmap, 0, 0},
	m_removeCursor {m_removeCursorPixmap, 0, 0},
	m_rightClickingMenu {new QMenu(cond->preProcessorWindow())},
	m_buildBankLinesAction {new QAction(GridCreatingConditionLaplace::tr("&Build left bank and right bank lines"), cond)},
	m_interpolateMenu {new QMenu(GridCreatingConditionLaplace::tr("&Interpolation Mode"), cond->preProcessorWindow())},
	m_interpolateSplineAction {new QAction(GridCreatingConditionLaplace::tr("&Spline"), cond)},
	m_interpolateLinearAction {new QAction(GridCreatingConditionLaplace::tr("&Linear"), cond)},
	m_addNewEdgeAction {new QAction(GridCreatingConditionLaplace::tr("Add &Division line"), cond)},
	m_joinRegionsAction {new QAction(GridCreatingConditionLaplace::tr("Remove D&ivision Line"), cond)},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GridCreatingConditionLaplace::tr("&Add Vertex"), cond)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GridCreatingConditionLaplace::tr("&Remove Vertex"), cond)},
	m_editCoordinatesAction {new QAction(GridCreatingConditionLaplace::tr("&Edit coordinates..."), cond)},
	m_wholeRegionDivisionSettingAction {new QAction(GridCreatingConditionLaplace::tr("Division Setting for &whole region..."), cond)},
	m_divisionSettingAction {new QAction(GridCreatingConditionLaplace::tr("&Division Setting for selected line..."), cond)},
	m_deploySettingAction {new QAction(GridCreatingConditionLaplace::tr("&Edge Points Deploying Setting..."), cond)},
	m_deploySubRegionSettingAction {new QAction(GridCreatingConditionLaplace::tr("&Deploying Setting for selected area..."), cond)},
	m_clearDivisionSettingAction {new QAction(GridCreatingConditionLaplace::tr("&Clear Division Setting..."), cond)},
	m_condition {cond}
{
	m_previewGridActor->SetMapper(m_previewGridMapper);
	m_previewGridActor->GetProperty()->SetRepresentationToWireframe();
	m_previewGridActor->GetProperty()->SetLighting(false);
	m_previewGridActor->GetProperty()->SetColor(0.2, 0.2, 0.2);
	m_previewGridActor->GetProperty()->SetLineWidth(1);
	m_previewGridActor->VisibilityOff();

	auto r = cond->renderer();
	r->AddActor(m_centerLine.pointsActor());
	r->AddActor(m_centerLineSpline.linesActor());

	r->AddActor(m_ctrlPointsController.pointsActor());
	r->AddActor(m_newEdgeLine.pointsActor());
	r->AddActor(m_newEdgeLine.linesActor());
	r->AddActor(m_previewGridActor);

	r->AddActor2D(m_upstreamActor.actor());
	r->AddActor2D(m_downstreamActor.actor());

	setupEdgeControllerForEdgeSelection(&m_ctrlPointsController, m_zDepthRange);

	m_centerLine.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
	m_ctrlPointsController.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
	m_newEdgeLine.pointsActor()->GetProperty()->SetPointSize(POINTSIZE);

	setupLabelActor(&m_upstreamActor);
	m_upstreamActor.setLabel("Upstream");
	setupLabelActor(&m_downstreamActor);
	m_downstreamActor.setLabel("Downstream");

	updateActorSetting();

	m_addNewEdgeAction->setCheckable(true);
	m_addVertexAction->setCheckable(true);
	m_removeVertexAction->setCheckable(true);
	m_interpolateSplineAction->setCheckable(true);
	m_interpolateLinearAction->setCheckable(true);

	m_interpolateMenu->addAction(m_interpolateSplineAction);
	m_interpolateMenu->addAction(m_interpolateLinearAction);

	connect(m_buildBankLinesAction, SIGNAL(triggered()), cond, SLOT(buildBankLines()));
	connect(m_interpolateSplineAction, SIGNAL(triggered()), cond, SLOT(interpolateModeSprine()));
	connect(m_interpolateLinearAction, SIGNAL(triggered()), cond, SLOT(interpolateModeLinear()));
	connect(m_addNewEdgeAction, SIGNAL(triggered(bool)), cond, SLOT(newEdgeMode(bool)));
	connect(m_joinRegionsAction, SIGNAL(triggered()), cond, SLOT(joinRegions()));
	connect(m_addVertexAction, SIGNAL(triggered(bool)), cond, SLOT(addVertexMode(bool)));
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), cond, SLOT(removeVertexMode(bool)));
	connect(m_editCoordinatesAction, SIGNAL(triggered()), cond, SLOT(editCoorinates()));
	connect(m_wholeRegionDivisionSettingAction, SIGNAL(triggered()), cond, SLOT(wholeRegionDivisionSetting()));
	connect(m_divisionSettingAction, SIGNAL(triggered()), cond, SLOT(divisionSetting()));
	connect(m_deploySettingAction, SIGNAL(triggered()), cond, SLOT(deploySetting()));
	connect(m_deploySubRegionSettingAction, SIGNAL(triggered()), cond, SLOT(subRegionDeploySetting()));
	connect(m_clearDivisionSettingAction, SIGNAL(triggered()), cond, SLOT(clearDivisionSetting()));
	connect(m_optimizeThread, SIGNAL(optimizeFinished()), cond, SLOT(informCommonRatioUpdate()));
	connect(m_buildThread, SIGNAL(buildFinished(int,int)), cond, SLOT(informPreviewGridPointsUpdate(int,int)));

	m_optimizeThread->start();
	m_buildThread->start();
}

GridCreatingConditionLaplace::Impl::~Impl()
{
	auto r = m_condition->renderer();
	r->RemoveActor(m_centerLine.pointsActor());
	r->RemoveActor(m_centerLineSpline.linesActor());
	r->RemoveActor(m_ctrlPointsController.pointsActor());
	r->RemoveActor(m_newEdgeLine.pointsActor());
	r->RemoveActor(m_newEdgeLine.linesActor());
	r->RemoveActor(m_previewGridActor);

	r->RemoveActor2D(m_upstreamActor.actor());
	r->RemoveActor2D(m_downstreamActor.actor());

	delete m_previewGrid;
	m_previewGridMapper->Delete();
	m_previewGridActor->Delete();

	clearCtrlPointsAndEdges();

	m_optimizeThread->finish();
	delete m_optimizeThread;

	m_buildThread->finish();
	delete m_buildThread;
}

Structured2DGrid* GridCreatingConditionLaplace::Impl::createGrid()
{
	if (m_editMode == EditMode::CenterLineOnly) {
		QMessageBox::warning(m_condition->preProcessorWindow(), GridCreatingConditionLaplace::tr("Warning"), GridCreatingConditionLaplace::tr("Please build left bank and right bank lines before creating grid."));
		return nullptr;
	}
	if (! divideSettingExists()) {
		bool ok = m_condition->wholeRegionDivisionSetting(true);
		if (! ok) {return nullptr;}
	}

	int iMax = m_ctrlPointCountI;
	for (auto div : m_divCountsStreamWise) {
		iMax += div - 1;
	}
	int jMax = m_ctrlPointCountJ;
	for (auto div : m_divCountsCrossSection) {
		jMax += div - 1;
	}

	auto grid = new Structured2DGrid(nullptr);
	auto gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*> (m_condition->m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);
	grid->setDimensions(iMax, jMax);

	auto points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(iMax * jMax);
	for (int idx = 0; idx < iMax * jMax; ++idx) {
		points->InsertNextPoint(0, 0, 0);
	}

	// Set CtrlPoints
	int jIdx = 0;
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		int iIdx = 0;
		for (int i = 0; i < m_ctrlPointCountI; ++i) {
			setGridPoint(points, ctrlPoint(i, j), iIdx, jIdx, iMax);
			if (i == m_ctrlPointCountI - 1) {break;}
			iIdx += m_divCountsStreamWise.at(i);
		}
		if (j == m_ctrlPointCountJ - 1) {break;}
		jIdx += m_divCountsCrossSection.at(j);
	}

	// Set points on edges
	jIdx = 0;
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		int iIdx = 0;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			auto line = edgeLineStreamWiseForDivisionPreview(i, j).polyLine();
			for (int k = 1; k < line.size() - 1; ++k) {
				const auto& p = line.at(k);
				int idx = iIdx + k + jIdx * iMax;
				points->SetPoint(idx, p.x(), p.y(), 0);
			}
			if (i == m_ctrlPointCountI - 1) {break;}
			iIdx += m_divCountsStreamWise.at(i);
		}
		if (j == m_ctrlPointCountJ - 1) {break;}
		jIdx += m_divCountsCrossSection.at(j);
	}

	int iIdx = 0;
	for (int i = 0; i < m_ctrlPointCountI; ++i) {
		jIdx = 0;
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			auto line = edgeLineCrossSectionForDivisionPreview(i, j).polyLine();
			for (int k = 1; k < line.size() - 1; ++k) {
				const auto& p = line.at(k);
				int idx = iIdx + (k + jIdx) * iMax;
				points->SetPoint(idx, p.x(), p.y(), 0);
			}
			if (j == m_ctrlPointCountJ - 1) {break;}
			jIdx += m_divCountsCrossSection.at(j);
		}
		if (i == m_ctrlPointCountI - 1) {break;}
		iIdx += m_divCountsStreamWise.at(i);
	}
	// Set Points for subregions
	int jOffset = 1;
	for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
		int iOffset = 1;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			auto edgeLineStreamWise1 = edgeLineStreamWiseForDivisionPreview(i, j).polyLine();
			auto edgeLineStreamWise2 = edgeLineStreamWiseForDivisionPreview(i, j + 1).polyLine();
			auto edgeLineCrossSection1 = edgeLineCrossSectionForDivisionPreview(i, j).polyLine();
			auto edgeLineCrossSection2 = edgeLineCrossSectionForDivisionPreview(i + 1, j).polyLine();
			auto subPoints = buildSubGrid(edgeLineStreamWise1, edgeLineStreamWise2, edgeLineCrossSection1, edgeLineCrossSection2, subRegionPoissonParameter(i, j));
			int idx = 0;
			for (int jj = 0; jj < m_divCountsCrossSection[j] - 1; ++jj) {
				for (int ii = 0; ii < m_divCountsStreamWise[i] - 1; ++ii) {
					setGridPoint(points, subPoints.at(idx), ii + iOffset, jj + jOffset, iMax);
					++idx;
				}
			}
			iOffset += m_divCountsStreamWise.at(i);
		}
		jOffset += m_divCountsCrossSection.at(j);
	}

	grid->vtkGrid()->SetPoints(points);
	points->Delete();

	for (auto c : grid->gridAttributes()) {
		c->allocate();
	}
	grid->setModified();
	return grid;
}

Structured2DGrid* GridCreatingConditionLaplace::Impl::createSubRegionGrid(int i, int j)
{
	int iMax = m_divCountsStreamWise[i] + 1;
	int jMax = m_divCountsCrossSection[j] + 1;

	auto grid = new Structured2DGrid(nullptr);
	grid->setDimensions(iMax, jMax);

	auto points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(iMax * jMax);
	for (int idx = 0; idx < iMax * jMax; ++idx) {
		points->InsertNextPoint(0, 0, 0);
	}

	// Set CtrlPoints
	int jIdx = 0;
	for (int jj = 0; jj < 2; ++jj) {
		int iIdx = 0;
		for (int ii = 0; ii < 2; ++ii) {
			setGridPoint(points, ctrlPoint(i + ii, j + jj), iIdx, jIdx, iMax);
			iIdx += m_divCountsStreamWise.at(i);
		}
		jIdx += m_divCountsCrossSection.at(j);
	}

	// Set points on edges
	jIdx = 0;
	for (int jj = 0; jj < 2; ++jj) {
		auto line = edgeLineStreamWiseForDivisionPreview(i, j + jj).polyLine();
		for (int k = 1; k < line.size() - 1; ++k) {
			const auto& p = line.at(k);
			int idx = k + jIdx * iMax;
			points->SetPoint(idx, p.x(), p.y(), 0);
		}
		jIdx += m_divCountsCrossSection.at(j);
	}

	int iIdx = 0;
	for (int ii = 0; ii < 2; ++ii) {
		auto line = edgeLineCrossSectionForDivisionPreview(i + ii, j).polyLine();
		for (int k = 1; k < line.size() - 1; ++k) {
			const auto& p = line.at(k);
			int idx = iIdx + k * iMax;
			points->SetPoint(idx, p.x(), p.y(), 0);
		}
		iIdx += m_divCountsStreamWise.at(i);
	}

	// Set Points for subregions
	int idx = 0;
	for (int jj = 0; jj < m_divCountsCrossSection[j] - 1; ++jj) {
		for (int ii = 0; ii < m_divCountsStreamWise[i] - 1; ++ii) {
			setGridPoint(points, m_previewGridPoints.at(idx), ii + 1, jj + 1, iMax);
			++idx;
		}
	}
	grid->vtkGrid()->SetPoints(points);
	points->Delete();
	grid->setModified();
	return grid;
}

std::vector<QPointF> GridCreatingConditionLaplace::Impl::buildSubGrid(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp)
{
	if (pp.setting == DeploySetting::Ratio) {
		return buildSubGridByRatio(edgeLineStreamWise1, edgeLineStreamWise2, edgeLineCrossSection1, edgeLineCrossSection2, pp);
	} else {
		return buildSubGridByPoisson(edgeLineStreamWise1, edgeLineStreamWise2, edgeLineCrossSection1, edgeLineCrossSection2, pp);
	}
}

std::vector<QPointF> GridCreatingConditionLaplace::Impl::buildSubGridByRatio(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp)
{
	std::vector<double> lenStreamWise1, lenStreamWise2, lenCrossSection1, lenCrossSection2;
	std::vector<double> p0, p1, q0, q1;
	lenStreamWise1 = buildLengths(edgeLineStreamWise1);
	lenStreamWise2 = buildLengths(edgeLineStreamWise2);
	lenCrossSection1 = buildLengths(edgeLineCrossSection1);
	lenCrossSection2 = buildLengths(edgeLineCrossSection2);

	p0 = setupRatios(lenStreamWise1);
	p1 = setupRatios(lenStreamWise2);
	q0 = setupRatios(lenCrossSection1);
	q1 = setupRatios(lenCrossSection2);

	int isize = edgeLineStreamWise1.size();
	int jsize = edgeLineCrossSection1.size();

	TmpGrid grid(isize, jsize);
	TmpGrid newGrid(isize, jsize);
	TmpGrid ratioGrid(isize, jsize);

	for (int j = 0; j < edgeLineCrossSection1.size(); ++j) {
		grid.x(0, j) = edgeLineCrossSection1.at(j).x();
		grid.y(0, j) = edgeLineCrossSection1.at(j).y();
		grid.x(isize - 1, j) = edgeLineCrossSection2.at(j).x();
		grid.y(isize - 1, j) = edgeLineCrossSection2.at(j).y();
	}
	for (int i = 0; i < edgeLineStreamWise1.size(); ++i) {
		grid.x(i, 0) = edgeLineStreamWise1.at(i).x();
		grid.y(i, 0) = edgeLineStreamWise1.at(i).y();
		grid.x(i, jsize - 1) = edgeLineStreamWise2.at(i).x();
		grid.y(i, jsize - 1) = edgeLineStreamWise2.at(i).y();
	}

	// initialize the grid
	for (int j = 1; j < edgeLineCrossSection1.size() - 1; ++j) {
		double r_c = (lenCrossSection1[j] / lenCrossSection1.back() + lenCrossSection2[j] / lenCrossSection2.back()) * 0.5;
		double s = j / static_cast<double>(jsize);
		for (int i = 1; i < edgeLineStreamWise1.size() - 1; ++i) {
			double r_s = (lenStreamWise1[i] / lenStreamWise1.back() + lenStreamWise2[i] / lenStreamWise2.back()) * 0.5;
			double r = i / static_cast<double>(isize);

			auto p_s = edgeLineStreamWise1.at(i) * (1 - r_c) + edgeLineStreamWise2.at(i) * r_c;
			grid.x(i, j) = p_s.x();
			grid.y(i, j) = p_s.y();

			ratioGrid.x(i, j) = p0[i] * (1 - s) + p1[i] * s;
			ratioGrid.y(i, j) = q0[j] * (1 - r) + q1[j] * r;
		}
	}

	// @todo loop count should be optimized
	double ax = 1;

	for (int c = 0; c < 1000; ++c) {
		newGrid = grid;
		for (int j = 1; j < jsize - 1; ++j) {
			for (int i = 1; i < isize - 1; ++i) {
				int i0 = i - 1;
				int i1 = i + 1;
				int j0 = j - 1;
				int j1 = j + 1;
				double p = ratioGrid.x(i, j);
				double q = ratioGrid.y(i, j);

				double x1 = (1 - p) * grid.x(i0, j) + p * grid.x(i1, j);
				double y1 = (1 - p) * grid.y(i0, j) + p * grid.y(i1, j);
				double dx1 = (grid.x(i1, j) - grid.x(i0 , j));
				double dy1 = (grid.y(i1, j) - grid.y(i0 , j));
				double len1 = dx1 * dx1 + dy1 * dy1;
				double x2 = (1 - q) * grid.x(i, j0) + q * grid.x(i, j1);
				double y2 = (1 - q) * grid.y(i, j0) + q * grid.y(i, j1);
				double dx2 = (grid.x(i, j1) - grid.x(i, j0));
				double dy2 = (grid.y(i, j1) - grid.y(i, j0));
				double len2 = dx2 * dx2 + dy2 * dy2;

				newGrid.x(i, j) = (x1 * len2 + x2 * len1) / (len1 + len2);
				newGrid.y(i, j) = (y1 * len2 + y2 * len1) / (len1 + len2);
			}
		}
		for (int j = 1; j < jsize - 1; ++j) {
			for (int i = 1; i < isize - 1; ++i) {
				grid.x(i, j) = ax * newGrid.x(i, j) + (1 - ax) * grid.x(i, j);
				grid.y(i, j) = ax * newGrid.y(i, j) + (1 - ax) * grid.y(i, j);
			}
		}
	}

	std::vector<QPointF> ret;
	for (int j = 1; j < jsize - 1; ++j) {
		for (int i = 1; i < isize - 1; ++i) {
			ret.push_back(QPointF(grid.x(i, j), grid.y(i, j)));
		}
	}

	return ret;
}

std::vector<QPointF> GridCreatingConditionLaplace::Impl::buildSubGridByPoisson(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp)
{
	std::vector<double> lenStreamWise1, lenStreamWise2, lenCrossSection1, lenCrossSection2;
	lenStreamWise1 = buildLengths(edgeLineStreamWise1);
	lenStreamWise2 = buildLengths(edgeLineStreamWise2);
	lenCrossSection1 = buildLengths(edgeLineCrossSection1);
	lenCrossSection2 = buildLengths(edgeLineCrossSection2);

	int isize = edgeLineStreamWise1.size();
	int jsize = edgeLineCrossSection1.size();

	TmpGrid grid(isize, jsize);
	TmpGrid newGrid(isize, jsize);
	TmpGrid PQGrid(isize, jsize);

	for (int j = 0; j < edgeLineCrossSection1.size(); ++j) {
		grid.x(0, j) = edgeLineCrossSection1.at(j).x();
		grid.y(0, j) = edgeLineCrossSection1.at(j).y();
		grid.x(isize - 1, j) = edgeLineCrossSection2.at(j).x();
		grid.y(isize - 1, j) = edgeLineCrossSection2.at(j).y();
	}
	for (int i = 0; i < edgeLineStreamWise1.size(); ++i) {
		grid.x(i, 0) = edgeLineStreamWise1.at(i).x();
		grid.y(i, 0) = edgeLineStreamWise1.at(i).y();
		grid.x(i, jsize - 1) = edgeLineStreamWise2.at(i).x();
		grid.y(i, jsize - 1) = edgeLineStreamWise2.at(i).y();
	}


	// initialize the grid
	for (int j = 1; j < edgeLineCrossSection1.size() - 1; ++j) {
		double r_c = (lenCrossSection1[j] / lenCrossSection1.back() + lenCrossSection2[j] / lenCrossSection2.back()) * 0.5;
		for (int i = 1; i < edgeLineStreamWise1.size() - 1; ++i) {
			double r_s = (lenStreamWise1[i] / lenStreamWise1.back() + lenStreamWise2[i] / lenStreamWise2.back()) * 0.5;

			auto p_s = edgeLineStreamWise1.at(i) * (1 - r_c) + edgeLineStreamWise2.at(i) * r_c;
			grid.x(i, j) = p_s.x();
			grid.y(i, j) = p_s.y();
		}
	}

	// @todo loop count should be optimized
	double ax = 0.3;

	double p = pp.manualP;
	double q = pp.manualQ;

	for (int c = 0; c < 1000; ++c) {
		newGrid = grid;
		for (int j = 1; j < jsize - 1; ++j) {
			for (int i = 1; i < isize - 1; ++i) {
				int i0 = i - 1;
				int i1 = i + 1;
				int j0 = j - 1;
				int j1 = j + 1;

				double dxdxi = grid.x(i1, j) - grid.x(i, j);
				double dydxi = grid.y(i1, j) - grid.y(i, j);
				double dxdeta = grid.x(i, j1) - grid.x(i, j);
				double dydeta = grid.y(i, j1) - grid.y(i, j);

				double alpha = dxdeta * dxdeta + dydeta * dydeta;
				double beta = dxdxi * dxdeta + dydxi * dydeta;
				double gamma = dxdxi * dxdxi + dydxi * dydxi;
				double sj = dxdxi * dydeta - dydxi * dxdeta;

				double w1, w2, w3, w4;

				w1 = alpha * (grid.x(i1, j) + grid.x(i0, j));
				w2 = -2 * beta * ((grid.x(i1, j1) - grid.x(i0, j1)) / 2 - (grid.x(i1, j0) - grid.x(i0, j0)) / 2) / 2;
				w3 = gamma * (grid.x(i, j1) + grid.x(i, j0));
				w4 = sj * (p * dxdxi + q * dxdeta);
				newGrid.x(i, j) = (w1 + w2 + w3 + w4) / (2 * alpha + 2 * gamma);

				w1 = alpha * (grid.y(i1, j) + grid.y(i0, j));
				w2 = -2 * beta * ((grid.y(i1, j1) - grid.y(i0, j1)) / 2 - (grid.y(i1, j0) - grid.y(i0, j0)) / 2) / 2;
				w3 = gamma * (grid.y(i, j1) + grid.y(i, j0));
				w4 = sj * (p * dydxi + q * dydeta);
				newGrid.y(i, j) = (w1 + w2 + w3 + w4) / (2 * alpha + 2 * gamma);
			}
		}
		// @todo calc error here.
		for (int j = 1; j < jsize - 1; ++j) {
			for (int i = 1; i < isize - 1; ++i) {
				grid.x(i, j) = ax * newGrid.x(i, j) + (1 - ax) * grid.x(i, j);
				grid.y(i, j) = ax * newGrid.y(i, j) + (1 - ax) * grid.y(i, j);
			}
		}
	}

	std::vector<QPointF> ret;
	for (int j = 1; j < jsize - 1; ++j) {
		for (int i = 1; i < isize - 1; ++i) {
			ret.push_back(QPointF(grid.x(i, j), grid.y(i, j)));
		}
	}

	return ret;
}

int GridCreatingConditionLaplace::Impl::ctrlPointCountI() const
{
	return m_ctrlPointCountI;
}

int GridCreatingConditionLaplace::Impl::ctrlpointCountJ() const
{
	return m_ctrlPointCountJ;
}

QPointF& GridCreatingConditionLaplace::Impl::ctrlPoint(int i, int j)
{
	return m_ctrlPoints[i + j * m_ctrlPointCountI];
}

bool GridCreatingConditionLaplace::Impl::divideSettingExists() const
{
	return m_manualDivSetting;
}

void GridCreatingConditionLaplace::Impl::copyCenterLine(GeoDataRiverSurvey* data, int num)
{
	GeoDataRiverPathPoint* p = data->headPoint();
	std::vector<QPointF> line;

	int numPoints = getNumPoints(data);
	int pointsCreated = 0;
	int idx = 0;

	while (p->nextPoint() != nullptr) {
		p = p->nextPoint();
		double r1 = pointsCreated / static_cast<double>(num - 1);
		double r2 = idx / static_cast<double>(numPoints - 1);
		if (idx == 0 || r1 <= r2) {
			auto pos = p->position();
			QPointF newPoint(pos.x(), pos.y());
			line.push_back(newPoint);
			++ pointsCreated;
		}
		++ idx;
	}
	m_centerLine.setPolyLine(line);
	if (line.size() < 2) {return;}

	m_editMode = EditMode::CenterLineOnly;
	m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::Normal;

	updateActorSetting();
	// updateLabelsAndSplines();
	// updateActionStatus();
}

int GridCreatingConditionLaplace::Impl::getNumPoints(GeoDataRiverSurvey* riverSurvey)
{
	int num = 0;
	GeoDataRiverPathPoint* p = riverSurvey->headPoint();
	while (p->nextPoint() != nullptr) {
		++ num;
		p = p->nextPoint();
	}

	return num;
}

GridCreatingConditionLaplace::Impl::InterpolationType& GridCreatingConditionLaplace::Impl::edgeInterpolationStreamWise(int i, int j)
{
	return m_edgeInterpolationStreamWise[i + j * (m_ctrlPointCountI - 1)];
}

GridCreatingConditionLaplace::Impl::InterpolationType& GridCreatingConditionLaplace::Impl::edgeInterpolationCrossSection(int i, int j)
{
	return m_edgeInterpolationCrossSection[i + j * m_ctrlPointCountI];
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineStreamWise(int i, int j)
{
	return *(m_edgeLinesStreamWise[i + j * (m_ctrlPointCountI - 1)]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineCrossSection(int i, int j)
{
	return *(m_edgeLinesCrossSection[i + j * m_ctrlPointCountI]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineStreamWiseForNodeSelection(int i, int j)
{
	return *(m_edgeLinesStreamWiseForNodeSelection[i + j * (m_ctrlPointCountI - 1)]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineCrossSectionForNodeSelection(int i, int j)
{
	return *(m_edgeLinesCrossSectionForNodeSelection[i + j * m_ctrlPointCountI]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineStreamWiseForEdgeSelection(int i, int j)
{
	return *(m_edgeLinesStreamWiseForEdgeSelection[i + j * (m_ctrlPointCountI - 1)]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineCrossSectionForEdgeSelection(int i, int j)
{
	return *(m_edgeLinesCrossSectionForEdgeSelection[i + j * m_ctrlPointCountI]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineStreamWiseForDivisionPreview(int i, int j)
{
	return *(m_edgeLinesStreamWiseForDivisionPreview[i + j * (m_ctrlPointCountI - 1)]);
}

PolyLineController& GridCreatingConditionLaplace::Impl::edgeLineCrossSectionForDivisionPreview(int i, int j)
{
	return *(m_edgeLinesCrossSectionForDivisionPreview[i + j * m_ctrlPointCountI]);
}

PolygonController& GridCreatingConditionLaplace::Impl::subRegionPolygons(int i, int j)
{
	return *(m_subRegionPolygons[i + j * (m_ctrlPointCountI - 1)]);
}

void GridCreatingConditionLaplace::Impl::insertEdgeLineStreamWise(PolyLineController* line, int idx)
{
	auto r = m_condition->renderer();
	r->AddActor(line->pointsActor());
	setupEdgeController(line, m_zDepthRange);
	if (idx == -1) {
		m_edgeLinesStreamWise.push_back(line);
	} else {
		m_edgeLinesStreamWise.insert(m_edgeLinesStreamWise.begin() + idx, line);
	}
}

void GridCreatingConditionLaplace::Impl::insertEdgeLineCrossSection(PolyLineController* line, int idx)
{
	auto r = m_condition->renderer();
	r->AddActor(line->pointsActor());
	setupEdgeController(line, m_zDepthRange);
	if (idx == -1) {
		m_edgeLinesCrossSection.push_back(line);
	} else {
		m_edgeLinesCrossSection.insert(m_edgeLinesCrossSection.begin() + idx, line);
	}
}

GridCreatingConditionLaplace::DivisionMode& GridCreatingConditionLaplace::Impl::divModeStreamWise(int i, int j)
{
	return m_divModesStreamWise[i + j * (m_ctrlPointCountI - 1)];
}

GridCreatingConditionLaplace::DivisionMode& GridCreatingConditionLaplace::Impl::divModeCrossSection(int i, int j)
{
	return m_divModesCrossSection[i + j * m_ctrlPointCountI];
}

double& GridCreatingConditionLaplace::Impl::divCommonRatioStreamWise(int i, int j)
{
	return m_divCommonRatiosStreamWise[i + j * (m_ctrlPointCountI - 1)];
}

double& GridCreatingConditionLaplace::Impl::divCommonRatioCrossSection(int i, int j)
{
	return m_divCommonRatiosCrossSection[i + j * m_ctrlPointCountI];
}

GridCreatingConditionLaplace::DeployParameter& GridCreatingConditionLaplace::Impl::subRegionPoissonParameter(int i, int j)
{
	return m_subRegionDeployParameters[i + j * (m_ctrlPointCountI - 1)];
}

PolyLineController* GridCreatingConditionLaplace::Impl::selectedSectionForEdgeSelection() const
{
	if (m_selectedSectionEdgeType == EdgeType::None) {
		return nullptr;
	} else if (m_selectedSectionEdgeType == EdgeType::StreamWise) {
		return m_edgeLinesStreamWiseForEdgeSelection[m_selectedSectionId];
	} else {
		return m_edgeLinesCrossSectionForEdgeSelection[m_selectedSectionId];
	}
}

GridCreatingConditionLaplace::Impl::InterpolationType GridCreatingConditionLaplace::Impl::selectedSectionInterpolateType() const
{
	if (m_selectedSectionEdgeType == EdgeType::None) {
		return InterpolationType::None;
	} else if (m_selectedSectionEdgeType == EdgeType::StreamWise) {
		return m_edgeInterpolationStreamWise[m_selectedSectionId];
	} else {
		return m_edgeInterpolationCrossSection[m_selectedSectionId];
	}
}

PolyLineController* GridCreatingConditionLaplace::Impl::hoveredSection() const
{
	if (m_hoveredSectionEdgeType == EdgeType::None) {
		return nullptr;
	} else if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		return m_edgeLinesStreamWise[m_hoveredSectionId];
	} else {
		return m_edgeLinesCrossSection[m_hoveredSectionId];
	}
}

PolyLineController* GridCreatingConditionLaplace::Impl::hoveredSectionForNodeSelection() const
{
	if (m_hoveredSectionEdgeType == EdgeType::None) {
		return nullptr;
	}	else if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		return m_edgeLinesStreamWiseForNodeSelection[m_hoveredSectionId];
	} else {
		return m_edgeLinesCrossSectionForNodeSelection[m_hoveredSectionId];
	}
}

PolyLineController* GridCreatingConditionLaplace::Impl::hoveredSectionForEdgeSelection() const
{
	if (m_hoveredSectionEdgeType == EdgeType::None) {
		return nullptr;
	}	else if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		return m_edgeLinesStreamWiseForEdgeSelection[m_hoveredSectionId];
	} else {
		return m_edgeLinesCrossSectionForEdgeSelection[m_hoveredSectionId];
	}
}

void GridCreatingConditionLaplace::Impl::removeEdgeLineStreamWise(int idx)
{
	auto col = m_condition->actorCollection();
	auto r = m_condition->renderer();

	auto start = m_edgeLinesStreamWise.begin() + (m_ctrlPointCountI - 1) * idx;
	auto end = start + (m_ctrlPointCountI - 1);
	for (auto it = start; it != end; ++it) {
		clearPolyline(*it, col, r);
		removeEdgeLinesStreamWiseForSelectionAndPreview(col, r);
	}
	m_edgeLinesStreamWise.erase(start, end);

	for (int i = 0; i < m_ctrlPointCountI; ++i) {
		auto line1 = m_edgeLinesCrossSection[i + (idx - 1) * m_ctrlPointCountI];
		auto line2 = m_edgeLinesCrossSection[i + (idx) * m_ctrlPointCountI];
		auto newPolyLine = line1->polyLine();
		for (auto p : line2->polyLine()) {
			newPolyLine.push_back(p);
		}
		line1->setPolyLine(newPolyLine);
		clearPolyline(line2, col, r);
		removeEdgeLinesCrossSectionForSelectionAndPreview(col, r);
	}
	auto start2 = m_edgeLinesCrossSection.begin() + (m_ctrlPointCountI) * idx;
	auto end2 = start2 + m_ctrlPointCountI;
	m_edgeLinesCrossSection.erase(start2, end2);

	auto start3 = m_ctrlPoints.begin() + m_ctrlPointCountI * idx;
	auto end3 = start3 + m_ctrlPointCountI;
	m_ctrlPoints.erase(start3, end3);

	m_divCountsCrossSection.erase(m_divCountsCrossSection.begin() + idx);

	auto r_start = m_subRegionDeployParameters.begin() + (m_ctrlPointCountI - 1) * idx;
	auto r_end = r_start + (m_ctrlPointCountI - 1);
	for (auto it = r_start; it != r_end; ++it) {
		removeSubRegionPolygon(col, r);
	}
	m_subRegionDeployParameters.erase(r_start, r_end);

	if (idx == m_centerLineIndex) {
		-- m_centerLineIndex;
		if (m_centerLineIndex == 0 && m_ctrlPointCountJ > 2) {
			m_centerLineIndex = 1;
		}
	} else if (idx < m_centerLineIndex) {
		--m_centerLineIndex;
	}
	m_ctrlPointCountJ -= 1;
	m_selectedSectionEdgeType = EdgeType::None;

	iRICUndoStack::instance().clear();
	updateEdgeLinesForSelection();
	m_condition->renderGraphicsView();
}

void GridCreatingConditionLaplace::Impl::removeEdgeLineCrossSection(int idx)
{
	auto col = m_condition->actorCollection();
	auto r = m_condition->renderer();

	for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
		auto it = m_edgeLinesCrossSection.begin() + m_ctrlPointCountI * (m_ctrlPointCountJ - 2 - j) + idx;
		clearPolyline(*it, col, r);
		m_edgeLinesCrossSection.erase(it);
		removeEdgeLinesCrossSectionForSelectionAndPreview(col, r);
	}
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		auto it = m_edgeLinesStreamWise.begin() + (m_ctrlPointCountI - 1) * (m_ctrlPointCountJ - 1 - j) + idx;
		auto line1 = *(it - 1);
		auto line2 = *it;
		auto newPolyLine = line1->polyLine();
		for (auto p : line2->polyLine()) {
			newPolyLine.push_back(p);
		}
		line1->setPolyLine(newPolyLine);
		clearPolyline(line2, col, r);
		m_edgeLinesStreamWise.erase(it);
		removeEdgeLinesStreamWiseForSelectionAndPreview(col, r);
	}
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		m_ctrlPoints.erase(m_ctrlPoints.begin() + m_ctrlPointCountI * (m_ctrlPointCountJ - 1 - j) + idx);
	}

	m_divCountsStreamWise.erase(m_divCountsStreamWise.begin() + idx);

	for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
		m_subRegionDeployParameters.erase(m_subRegionDeployParameters.begin() + (m_ctrlPointCountI - 1) * (m_ctrlPointCountJ - 2 - j) + idx);
		removeSubRegionPolygon(col, r);
	}

	m_ctrlPointCountI -= 1;
	m_selectedSectionEdgeType = EdgeType::None;

	iRICUndoStack::instance().clear();
	updateEdgeLinesForSelection();
	m_condition->renderGraphicsView();
}

void GridCreatingConditionLaplace::Impl::updateActorSetting()
{
	m_centerLine.pointsActor()->VisibilityOff();
	m_centerLineSpline.linesActor()->VisibilityOff();
	m_newEdgeLine.pointsActor()->VisibilityOff();
	m_newEdgeLine.linesActor()->VisibilityOff();
	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();

	m_ctrlPointsController.pointsActor()->VisibilityOff();
	for (auto line : m_edgeLinesStreamWise) {
		line->pointsActor()->VisibilityOff();
	}
	for (auto line : m_edgeLinesStreamWiseForEdgeSelection) {
		line->linesActor()->GetProperty()->SetLineWidth(1);
		line->linesActor()->VisibilityOff();
	}
	for (auto line : m_edgeLinesStreamWiseForDivisionPreview) {
		line->pointsActor()->VisibilityOff();
	}
	for (auto line : m_edgeLinesCrossSection) {
		line->pointsActor()->VisibilityOff();
	}
	for (auto line : m_edgeLinesCrossSectionForEdgeSelection) {
		line->linesActor()->GetProperty()->SetLineWidth(1);
		line->linesActor()->VisibilityOff();
	}
	for (auto line : m_edgeLinesCrossSectionForDivisionPreview) {
		line->pointsActor()->VisibilityOff();
	}
	for (auto polygon : m_subRegionPolygons) {
		polygon->paintActor()->VisibilityOff();
	}

	auto col = m_condition->actorCollection();
	auto col2 = m_condition->actor2DCollection();
	col->RemoveAllItems();
	col2->RemoveAllItems();

	if (m_editMode == EditMode::CenterLineOnly) {
		if (m_itemSelected) {
			col->AddItem(m_centerLine.pointsActor());
		}
		col->AddItem(m_centerLineSpline.linesActor());
		setupLabelsForCenterLine(col2);
	} else {
		if (m_itemSelected) {
			col->AddItem(m_ctrlPointsController.pointsActor());
			col->AddItem(m_newEdgeLine.pointsActor());
		}
		col->AddItem(m_newEdgeLine.linesActor());

		if (m_itemSelected) {
			for (auto line : m_edgeLinesStreamWise) {
				col->AddItem(line->pointsActor());
			}
		}
		for (auto line : m_edgeLinesStreamWiseForEdgeSelection) {
			col->AddItem(line->linesActor());
		}
		if (m_itemSelected) {
			for (auto line : m_edgeLinesCrossSection) {
				col->AddItem(line->pointsActor());
			}
		}
		if (m_editMode == EditMode::RegionDefined && m_itemSelected) {
			for (auto line : m_edgeLinesStreamWiseForDivisionPreview) {
				col->AddItem(line->pointsActor());
			}
			for (auto line : m_edgeLinesCrossSectionForDivisionPreview) {
				col->AddItem(line->pointsActor());
			}
		}
		for (auto line : m_edgeLinesCrossSectionForEdgeSelection) {
			col->AddItem(line->linesActor());
		}
		if (m_itemSelected) {
			auto selectedEdge = selectedSectionForEdgeSelection();
			if (selectedEdge != nullptr) {
				selectedEdge->linesActor()->GetProperty()->SetLineWidth(3);
			}
			if (m_selectedSubRegionId != -1) {
				auto region = m_subRegionPolygons[m_selectedSubRegionId];
				col->AddItem(region->paintActor());
			}
		}
		col2->AddItem(m_upstreamActor.actor());
		col2->AddItem(m_downstreamActor.actor());
	}
	m_condition->updateVisibilityWithoutRendering();
}

void GridCreatingConditionLaplace::Impl::updateActionStatus()
{
	m_addNewEdgeAction->setChecked(false);
	m_addNewEdgeAction->setEnabled(false);
	m_addVertexAction->setChecked(false);
	m_addVertexAction->setEnabled(true);
	m_removeVertexAction->setChecked(false);
	m_removeVertexAction->setEnabled(true);
	m_deploySubRegionSettingAction->setEnabled(false);

	m_interpolateMenu->setEnabled(false);
	m_interpolateSplineAction->setChecked(false);
	m_interpolateLinearAction->setChecked(false);

	if (m_editMode == EditMode::CenterLineOnly) {
		m_editCoordinatesAction->setEnabled(true);
		switch (m_centerLineOnlyMouseEventMode) {
		case CenterLineOnlyMouseEventMode::BeforeDefining:
		case CenterLineOnlyMouseEventMode::Defining:
			m_addVertexAction->setEnabled(false);
			m_removeVertexAction->setEnabled(false);
			break;
		case CenterLineOnlyMouseEventMode::AddVertexPrepare:
		case CenterLineOnlyMouseEventMode::AddVertexNotPossible:
		case CenterLineOnlyMouseEventMode::AddVertex:
			m_addVertexAction->setChecked(true);
			break;
		case CenterLineOnlyMouseEventMode::RemoveVertexPrepare:
		case CenterLineOnlyMouseEventMode::RemoveVertexNotPossible:
			m_removeVertexAction->setChecked(true);
			break;
		default:
			break;
		}
	} else {
		bool lineSelected = (m_selectedSectionEdgeType != EdgeType::None);

		m_addNewEdgeAction->setEnabled(true);
		m_joinRegionsAction->setEnabled(lineSelected);
		m_editCoordinatesAction->setEnabled(lineSelected);
		m_divisionSettingAction->setEnabled(lineSelected);
		m_deploySettingAction->setEnabled(lineSelected);
		m_deploySubRegionSettingAction->setEnabled(m_selectedSubRegionId != -1);

		if (selectedSectionInterpolateType() == InterpolationType::Spline) {
			m_interpolateSplineAction->setChecked(true);
		} else if (selectedSectionInterpolateType() == InterpolationType::Linear) {
			m_interpolateLinearAction->setChecked(true);
		}
		switch (m_regionDefinedMouseEventMode) {
		case RegionDefinedMouseEventMode::AddEdgeLine:
		case RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare:
		case RegionDefinedMouseEventMode::AddEdgeLineNotPossible:
		case RegionDefinedMouseEventMode::AddEdgeLinePrepare:
			m_addNewEdgeAction->setChecked(true);
			m_addVertexAction->setEnabled(false);
			m_removeVertexAction->setEnabled(false);
			break;
		case RegionDefinedMouseEventMode::AddVertexPrepare:
		case RegionDefinedMouseEventMode::AddVertexNotPossible:
		case RegionDefinedMouseEventMode::AddVertex:
			m_addVertexAction->setChecked(true);
			break;
		case RegionDefinedMouseEventMode::RemoveVertexPrepare:
		case RegionDefinedMouseEventMode::RemoveVertexNotPossible:
			m_removeVertexAction->setChecked(true);
			break;
		default:
			break;
		}
	}
}

void GridCreatingConditionLaplace::Impl::updateMouseEventMode(const QPoint& pos, bool updateSelect)
{
	double x = pos.x();
	double y = pos.y();
	auto v = m_condition->graphicsView();
	v->viewportToWorld(x, y);
	QPointF worldPos(x, y);
	double radius = v->stdRadius(iRIC::nearRadius());

	if (m_editMode == EditMode::CenterLineOnly) {
		switch (m_centerLineOnlyMouseEventMode) {
		case CenterLineOnlyMouseEventMode::AddVertexPrepare:
		case CenterLineOnlyMouseEventMode::AddVertexNotPossible:
			if (m_centerLineSpline.isEdgeSelectable(worldPos, radius, &m_hoveredEdgeId)) {
				m_hoveredEdgeId /= SUBDIVNUM;
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::AddVertexPrepare;
			} else {
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::AddVertexNotPossible;
			}
			break;
		case CenterLineOnlyMouseEventMode::RemoveVertexPrepare:
		case CenterLineOnlyMouseEventMode::RemoveVertexNotPossible:
			if (m_centerLine.isVertexSelectable(worldPos, radius, &m_hoveredVertexId)) {
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::RemoveVertexPrepare;
			} else {
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::RemoveVertexNotPossible;
			}
			break;
		case CenterLineOnlyMouseEventMode::Normal:
		case CenterLineOnlyMouseEventMode::MoveVertex:
		case CenterLineOnlyMouseEventMode::MoveVertexPrepare:
		case CenterLineOnlyMouseEventMode::AddVertex:
			if (m_centerLine.isVertexSelectable(worldPos, radius, &m_hoveredVertexId)) {
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::MoveVertexPrepare;
			} else {
				m_centerLineOnlyMouseEventMode = CenterLineOnlyMouseEventMode::Normal;
			}
			break;
		default:
			break;
		}
	} else {
		PolyLineController* edge = nullptr;
		switch (m_regionDefinedMouseEventMode) {
		case RegionDefinedMouseEventMode::AddVertexPrepare:
		case RegionDefinedMouseEventMode::AddVertexNotPossible:
			edge = updateHoveredSection(pos);
			if (edge != nullptr && edge->isEdgeSelectable(worldPos, radius, &m_hoveredEdgeId)) {
				m_hoveredEdgeId /= SUBDIVNUM;
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddVertexPrepare;
			} else {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddVertexNotPossible;
			}
			break;
		case RegionDefinedMouseEventMode::RemoveVertexPrepare:
		case RegionDefinedMouseEventMode::RemoveVertexNotPossible:
			updateHoveredSection(pos);
			edge = hoveredSectionForNodeSelection();
			if (edge != nullptr && edge->isVertexSelectable(worldPos, radius, &m_hoveredVertexId)) {
				if (m_hoveredVertexId != 0 && m_hoveredVertexId != edge->polyLine().size() - 1) {
					m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::RemoveVertexPrepare;
				}
			} else {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::RemoveVertexNotPossible;
			}
			break;
		case RegionDefinedMouseEventMode::AddEdgeLinePrepare:
		case RegionDefinedMouseEventMode::AddEdgeLineNotPossible:
			edge = updateHoveredSection(pos);
			if (edge != nullptr && edge->isEdgeSelectable(worldPos, radius, &m_newEdgeStartEdgeId) && setupNewEdgeSetting()) {
				m_newEdgeStartEdgeId /= SUBDIVNUM;
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddEdgeLinePrepare;
			} else {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddEdgeLineNotPossible;
			}
			break;
		case RegionDefinedMouseEventMode::AddEdgeLine:
		case RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare:
			edge = updateHoveredSection(pos);
			if (edge != nullptr && edge->isEdgeSelectable(worldPos, radius, &m_newEdgeEndEdgeId) && isNewEdgeFinishReady()) {
				m_newEdgeEndEdgeId /= SUBDIVNUM;
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare;
			} else {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::AddEdgeLine;
			}
			break;
		case RegionDefinedMouseEventMode::Normal:
		case RegionDefinedMouseEventMode::MoveVertex:
		case RegionDefinedMouseEventMode::MoveVertexPrepare:
		case RegionDefinedMouseEventMode::AddVertex:
			updateHoveredSection(pos);
			edge = hoveredSectionForNodeSelection();
			if (edge != nullptr && edge->isVertexSelectable(worldPos, radius, &m_hoveredVertexId)) {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::MoveVertexPrepare;
			} else {
				m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::Normal;

				if (updateSelect && iRIC::isNear(m_pressPos, pos)) {
					m_selectedSectionEdgeType = m_hoveredSectionEdgeType;
					m_selectedSectionId = m_hoveredSectionId;
					m_selectedSubRegionId = -1;
					if (m_selectedSectionEdgeType == EdgeType::None) {
						updateSelectedSubRegion(pos);
					}
				}
			}
			break;
		default:
			break;
		}
		updateActorSetting();
		m_condition->renderGraphicsView();
	}
}

PolyLineController* GridCreatingConditionLaplace::Impl::updateHoveredSection(const QPoint& pos)
{
	double x = pos.x();
	double y = pos.y();
	auto v = m_condition->graphicsView();
	v->viewportToWorld(x, y);
	QPointF worldPos(x, y);
	double radius = v->stdRadius(iRIC::nearRadius());
	int edgeId;

	m_hoveredSectionEdgeType = EdgeType::None;
	m_hoveredSectionId = -1;
	for (int i = 0; i < m_edgeLinesStreamWiseForEdgeSelection.size() && m_hoveredSectionEdgeType == EdgeType::None; ++i) {
		auto edge = m_edgeLinesStreamWiseForEdgeSelection[i];
		if (edge->isEdgeSelectable(worldPos, radius, &edgeId)) {
			m_hoveredSectionEdgeType = EdgeType::StreamWise;
			m_hoveredSectionId = i;
		}
	}
	for (int i = 0; i < m_edgeLinesCrossSectionForEdgeSelection.size() && m_hoveredSectionEdgeType == EdgeType::None; ++i) {
		auto edge = m_edgeLinesCrossSectionForEdgeSelection[i];
		if (edge->isEdgeSelectable(worldPos, radius, &edgeId)) {
			m_hoveredSectionEdgeType = EdgeType::CrossSection;
			m_hoveredSectionId = i;
		}
	}

	return hoveredSectionForEdgeSelection();
}

PolygonController* GridCreatingConditionLaplace::Impl::updateSelectedSubRegion(const QPoint& pos)
{
	double x = pos.x();
	double y = pos.y();
	auto v = m_condition->graphicsView();
	v->viewportToWorld(x, y);
	QPointF worldPos(x, y);

	m_selectedSubRegionId = -1;
	for (int i = 0; i < m_subRegionPolygons.size(); ++i) {
		auto region = m_subRegionPolygons.at(i);
		if (region->isAreaSelectable(worldPos)) {
			m_selectedSubRegionId = i;
			break;
		}
	}
	if (m_selectedSubRegionId == -1) {
		return nullptr;
	}
	return m_subRegionPolygons[m_selectedSubRegionId];
}

bool GridCreatingConditionLaplace::Impl::setupNewEdgeSetting()
{
	if (m_hoveredSectionEdgeType == EdgeType::None) {
		return false;
	}
	m_newEdgeType = m_hoveredSectionEdgeType;

	if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		int i = m_hoveredSectionId % (m_ctrlPointCountI - 1);
		int j = (m_hoveredSectionId - i) / (m_ctrlPointCountI - 1);
		m_newEdgeCtrlPointId = i + 1;
		if (j == 0) {
			m_newEdgeFromZero = true;
			return true;
		} else if (j == m_ctrlPointCountJ - 1) {
			m_newEdgeFromZero = false;
			return true;
		}
		return false;
	}	else if (m_hoveredSectionEdgeType == EdgeType::CrossSection) {
		int j = m_hoveredSectionId / m_ctrlPointCountI;
		int i = m_hoveredSectionId % m_ctrlPointCountI;
		m_newEdgeCtrlPointId = j + 1;
		if (i == 0) {
			m_newEdgeFromZero = true;
			return true;
		} else if (i == m_ctrlPointCountI - 1) {
			m_newEdgeFromZero = false;
			return true;
		}
		return false;
	} else {
		return false;
	}
}

bool GridCreatingConditionLaplace::Impl::isNewEdgeFinishReady()
{
	if (m_newEdgeType != m_hoveredSectionEdgeType) {return false;}
	if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		int i = m_hoveredSectionId % (m_ctrlPointCountI - 1);
		int j = (m_hoveredSectionId - i) / (m_ctrlPointCountI - 1);
		if (i + 1 != m_newEdgeCtrlPointId) {return false;}
		if (j == 0 && (! m_newEdgeFromZero)){
			return true;
		} else if (j == m_ctrlPointCountJ - 1 && m_newEdgeFromZero) {
			return true;
		} else {
			return false;
		}
	}	else if (m_hoveredSectionEdgeType == EdgeType::CrossSection) {
		int j = m_hoveredSectionId / m_ctrlPointCountI;
		int i = m_hoveredSectionId % m_ctrlPointCountI;
		if (j + 1 != m_newEdgeCtrlPointId) {return false;}
		if (i == 0 && (! m_newEdgeFromZero)) {
			return true;
		} else if (i == m_ctrlPointCountI - 1 && m_newEdgeFromZero) {
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

void GridCreatingConditionLaplace::Impl::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	if (m_editMode == EditMode::CenterLineOnly) {
		switch (m_centerLineOnlyMouseEventMode) {
		case CenterLineOnlyMouseEventMode::BeforeDefining:
		case CenterLineOnlyMouseEventMode::Defining:
			v->setCursor(Qt::CrossCursor);
			break;
		case CenterLineOnlyMouseEventMode::AddVertex:
		case CenterLineOnlyMouseEventMode::MoveVertex:
			v->setCursor(Qt::ClosedHandCursor);
			break;
		case CenterLineOnlyMouseEventMode::MoveVertexPrepare:
			v->setCursor(Qt::OpenHandCursor);
			break;
		case CenterLineOnlyMouseEventMode::AddVertexPrepare:
			v->setCursor(m_addCursor);
			break;
		case CenterLineOnlyMouseEventMode::RemoveVertexPrepare:
			v->setCursor(m_removeCursor);
			break;
		default:
			v->setCursor(Qt::ArrowCursor);
		}
	} else {
		switch (m_regionDefinedMouseEventMode) {
		case RegionDefinedMouseEventMode::AddVertex:
		case RegionDefinedMouseEventMode::MoveVertex:
			v->setCursor(Qt::ClosedHandCursor);
			break;
		case RegionDefinedMouseEventMode::MoveVertexPrepare:
			v->setCursor(Qt::OpenHandCursor);
			break;
		case RegionDefinedMouseEventMode::AddEdgeLinePrepare:
		case RegionDefinedMouseEventMode::AddEdgeLineFinishPrepare:
		case RegionDefinedMouseEventMode::AddVertexPrepare:
			v->setCursor(m_addCursor);
			break;
		case RegionDefinedMouseEventMode::AddEdgeLine:
			v->setCursor(Qt::CrossCursor);
			break;
		case RegionDefinedMouseEventMode::RemoveVertexPrepare:
			v->setCursor(m_removeCursor);
			break;
		default:
			v->setCursor(Qt::ArrowCursor);
		}
	}
}

void GridCreatingConditionLaplace::Impl::buildBankLines()
{
	if (m_editMode == EditMode::CenterLineOnly) {
		if (m_centerLine.polyLine().size() < 2) {
			QMessageBox::warning(m_condition->preProcessorWindow(), tr("Warning"), tr("Center Line is not defined yet."));
			return;
		}
	} else {
		std::vector<QPointF> line;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			line.push_back(ctrlPoint(i, m_centerLineIndex));
			for (auto p : edgeLineStreamWise(i, m_centerLineIndex).polyLine()) {
				line.push_back(p);
			}
		}
		line.push_back(ctrlPoint(m_ctrlPointCountI - 1, m_centerLineIndex));
		m_centerLine.setPolyLine(line);
	}
	GridCreatingConditionLaplaceBuildBankLinesDialog dialog(m_condition->preProcessorWindow());
	dialog.setLeftBankDistance(m_previousLeftBankDistance);
	dialog.setRightBankDistance(m_previousRightBankDistance);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	double leftDist = dialog.leftBankDistance();
	double rightDist = dialog.rightBankDistance();

	std::vector<QPointF> leftBank, rightBank;

	BankLinesBuilder::buildBankLines(m_centerLine.polyLine(), leftDist, rightDist, &leftBank, &rightBank);

	m_centerLine.pointsActor()->VisibilityOff();
	m_centerLineSpline.linesActor()->VisibilityOff();

	m_editMode = Impl::EditMode::RegionDefined;

	clearCtrlPointsAndEdges();

	m_centerLineIndex = 1;
	m_ctrlPointCountI = 2;
	m_ctrlPointCountJ = 3;

	auto center = m_centerLine.polyLine();

	m_ctrlPoints.push_back(rightBank.front());
	m_ctrlPoints.push_back(rightBank.back());
	m_ctrlPoints.push_back(center.front());
	m_ctrlPoints.push_back(center.back());
	m_ctrlPoints.push_back(leftBank.front());
	m_ctrlPoints.push_back(leftBank.back());

	auto r = m_condition->renderer();

	for (int i = 0; i < 3; ++i) {
		auto line = new PolyLineController();
		setupEdgeController(line, m_zDepthRange);
		r->AddActor(line->pointsActor());
		m_edgeLinesStreamWise.push_back(line);

		m_edgeInterpolationStreamWise.push_back(InterpolationType::Spline);

		addEdgeLinesStreamWiseForSelectionAndPreview(r);
	}
	for (int i = 0; i < 4; ++i) {
		auto line = new PolyLineController();
		setupEdgeController(line, m_zDepthRange);
		r->AddActor(line->pointsActor());
		m_edgeLinesCrossSection.push_back(line);

		m_edgeInterpolationCrossSection.push_back(InterpolationType::Linear);

		addEdgeLinesCrossSectionForSelectionAndPreview(r);
	}
	for (int i = 0; i < 2; ++i) {
		DeployParameter p;
		m_subRegionDeployParameters.push_back(p);
		addSubRegionPolygon(r);
	}

	rightBank.pop_back();
	rightBank.erase(rightBank.begin());
	m_edgeLinesStreamWise[0]->setPolyLine(rightBank);
	center.pop_back();
	center.erase(center.begin());
	m_edgeLinesStreamWise[1]->setPolyLine(center);
	leftBank.pop_back();
	leftBank.erase(leftBank.begin());
	m_edgeLinesStreamWise[2]->setPolyLine(leftBank);

	m_divCountsCrossSection.assign(2, 1);
	m_divCountsStreamWise.assign(1, 1);
	m_divModesCrossSection.assign(4, DivisionMode::Auto);
	m_divModesStreamWise.assign(3, DivisionMode::Auto);
	m_divCommonRatiosCrossSection.assign(4, 1);
	m_divCommonRatiosStreamWise.assign(3, 1);

	updateEdgeLinesForSelection();

	updateActorSetting();
	updateActionStatus();
	m_condition->setupMenu();

	m_previousLeftBankDistance = leftDist;
	m_previousRightBankDistance = rightDist;
	iRICUndoStack::instance().clear();
}

void GridCreatingConditionLaplace::Impl::addNewEdge(const QPoint &pos, PreProcessorGraphicsViewInterface* v)
{
	double x = pos.x();
	double y = pos.y();
	v->viewportToWorld(x, y);
	QPointF clickedP(x, y);

	QPointF leg;
	bool ok = getPerpendicularLineOfLeg(clickedP, *hoveredSectionForEdgeSelection(), &leg);
	if (! ok) {return;}

	auto polyline = m_newEdgeLine.polyLine();
	polyline.pop_back();
	polyline.push_back(leg);

	int startEdgeId = m_newEdgeStartEdgeId;
	int endEdgeId = m_newEdgeEndEdgeId;
	if (! m_newEdgeFromZero) {
		std::reverse(polyline.begin(), polyline.end());
		std::swap(startEdgeId, endEdgeId);
	}

	std::vector<LinePair> dividedLines;
	std::vector<std::vector<QPointF> > newLines;
	std::vector<QPointF> newCtrlPoints;
	auto renderer = m_condition->renderer();

	if (m_newEdgeType == EdgeType::StreamWise) {
		newCtrlPoints.push_back(polyline.front());
		dividedLines.push_back(divideLine(edgeLineStreamWise(m_newEdgeCtrlPointId - 1, 0).polyLine(), startEdgeId));
		newCtrlPoints.push_back(polyline.back());
		dividedLines.push_back(divideLine(edgeLineStreamWise(m_newEdgeCtrlPointId - 1, m_ctrlPointCountJ - 1).polyLine(), endEdgeId));

		auto it = polyline.begin() + 1;
		for (int j = 1; j < m_ctrlPointCountJ - 1; ++j) {
			auto lineToDivide = edgeLineStreamWiseForEdgeSelection(m_newEdgeCtrlPointId - 1, j).polyLine();

			int edgeId;
			QPointF xsec;
			std::vector<QPointF> newLine;
			while (it != polyline.end() && ! getIntersection(*(it - 1), *it, lineToDivide, &edgeId, &xsec)) {
				newLine.push_back(*it);
				++it;
			}
			newCtrlPoints.insert(newCtrlPoints.begin() + j, xsec);
			edgeId /= SUBDIVNUM;
			dividedLines.insert(dividedLines.begin() + j, divideLine(edgeLineStreamWise(m_newEdgeCtrlPointId - 1, j).polyLine(), edgeId));
			newLines.push_back(newLine);
		}
		std::vector<QPointF> newLine;
		while (it != polyline.end() - 1) {
			newLine.push_back(*it);
			++it;
		}
		newLines.push_back(newLine);
		for (int j = 0; j < m_ctrlPointCountJ; ++j) {
			m_ctrlPoints.insert(m_ctrlPoints.begin() + m_newEdgeCtrlPointId + j * (m_ctrlPointCountI + 1), newCtrlPoints[j]);

			m_edgeLinesStreamWise[m_newEdgeCtrlPointId - 1 + j * m_ctrlPointCountI]->setPolyLine(dividedLines[j].line1);
			auto newPolyLineController = new PolyLineController();
			newPolyLineController->setPolyLine(dividedLines[j].line2);
			insertEdgeLineStreamWise(newPolyLineController, m_newEdgeCtrlPointId + j * m_ctrlPointCountI);
			m_edgeInterpolationStreamWise.insert(m_edgeInterpolationStreamWise.begin() + m_newEdgeCtrlPointId + j * m_ctrlPointCountI, edgeInterpolationStreamWise(m_newEdgeCtrlPointId - 1, j));
			m_divModesStreamWise.insert(m_divModesStreamWise.begin() + m_newEdgeCtrlPointId + j * m_ctrlPointCountI, divModeStreamWise(m_newEdgeCtrlPointId - 1, j));
			m_divCommonRatiosStreamWise.insert(m_divCommonRatiosStreamWise.begin() + m_newEdgeCtrlPointId + j * m_ctrlPointCountI, divCommonRatioStreamWise(m_newEdgeCtrlPointId - 1, j));

			addEdgeLinesStreamWiseForSelectionAndPreview(renderer);
		}
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			auto newPolyLineController = new PolyLineController();
			newPolyLineController->setPolyLine(newLines.at(j));
			insertEdgeLineCrossSection(newPolyLineController, m_newEdgeCtrlPointId + j * (m_ctrlPointCountI + 1));
			m_edgeInterpolationCrossSection.insert(m_edgeInterpolationCrossSection.begin() + m_newEdgeCtrlPointId + j * (m_ctrlPointCountI + 1), InterpolationType::Linear);
			m_divModesCrossSection.insert(m_divModesCrossSection.begin() + m_newEdgeCtrlPointId + j * (m_ctrlPointCountI + 1), DivisionMode::Auto);
			m_divCommonRatiosCrossSection.insert(m_divCommonRatiosCrossSection.begin() + m_newEdgeCtrlPointId + j * (m_ctrlPointCountI + 1), 1);

			addEdgeLinesCrossSectionForSelectionAndPreview(renderer);
		}

		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			auto it = m_subRegionDeployParameters.begin() + m_newEdgeCtrlPointId - 1 + j * m_ctrlPointCountI;
			auto pp = *it;
			m_subRegionDeployParameters.insert(it + 1, pp);
			addSubRegionPolygon(renderer);
		}

		m_divCountsStreamWise.insert(m_divCountsStreamWise.begin() + m_newEdgeCtrlPointId, 1);
		++m_ctrlPointCountI;
		updateEdgeLinesForSelection();

		std::vector<PolyLineController*> lines1, lines2;
		for (int j = 0; j < m_ctrlPointCountJ; ++j) {
			lines1.push_back(&(edgeLineStreamWiseForEdgeSelection(m_newEdgeCtrlPointId - 1, j)));
			lines2.push_back(&(edgeLineStreamWiseForEdgeSelection(m_newEdgeCtrlPointId, j)));
		}
		double len1 = averageLength(lines1);
		double len2 = averageLength(lines2);
		int newDivCount = m_divCountsStreamWise[m_newEdgeCtrlPointId - 1] + 1;
		int newDivCounts1 = static_cast<int>(newDivCount * len1 / (len1 + len2));
		if (newDivCounts1 == 0) {newDivCounts1 = 1;}
		int newDivCounts2 = newDivCount - newDivCounts1;
		if (newDivCounts2 == 0) {newDivCounts2 = 1;}

		m_divCountsStreamWise[m_newEdgeCtrlPointId - 1] = newDivCounts1;
		m_divCountsStreamWise[m_newEdgeCtrlPointId] = newDivCounts2;
	} else {
		newCtrlPoints.push_back(polyline.front());
		dividedLines.push_back(divideLine(edgeLineCrossSection(0, m_newEdgeCtrlPointId - 1).polyLine(), startEdgeId));
		newCtrlPoints.push_back(polyline.back());
		dividedLines.push_back(divideLine(edgeLineCrossSection(m_ctrlPointCountI - 1, m_newEdgeCtrlPointId - 1).polyLine(), endEdgeId));

		auto it = polyline.begin() + 1;
		for (int i = 1; i < m_ctrlPointCountI - 1; ++i) {
			auto lineToDivide = edgeLineCrossSectionForEdgeSelection(i, m_newEdgeCtrlPointId - 1).polyLine();

			int edgeId;
			QPointF xsec;
			std::vector<QPointF> newLine;
			while (it != polyline.end() && ! getIntersection(*(it - 1), *it, lineToDivide, &edgeId, &xsec)) {
				newLine.push_back(*it);
				++it;
			}
			newCtrlPoints.insert(newCtrlPoints.begin() + i, xsec);
			edgeId /= SUBDIVNUM;
			dividedLines.insert(dividedLines.begin() + i, divideLine(edgeLineCrossSection(i, m_newEdgeCtrlPointId - 1).polyLine(), edgeId));
			newLines.push_back(newLine);
		}
		std::vector<QPointF> newLine;
		while (it != polyline.end() - 1) {
			newLine.push_back(*it);
			++it;
		}
		newLines.push_back(newLine);
		for (int i = 0; i < m_ctrlPointCountI; ++i) {
			m_ctrlPoints.insert(m_ctrlPoints.begin() + i + m_newEdgeCtrlPointId * m_ctrlPointCountI, newCtrlPoints[i]);

			m_edgeLinesCrossSection[i + (m_newEdgeCtrlPointId - 1) * m_ctrlPointCountI]->setPolyLine(dividedLines[i].line1);
			auto newPolyLineController = new PolyLineController();
			newPolyLineController->setPolyLine(dividedLines[i].line2);
			insertEdgeLineCrossSection(newPolyLineController, i + m_newEdgeCtrlPointId * m_ctrlPointCountI);
			m_edgeInterpolationCrossSection.insert(m_edgeInterpolationCrossSection.begin() + i * m_newEdgeCtrlPointId + m_ctrlPointCountI, edgeInterpolationCrossSection(i, m_newEdgeCtrlPointId - 1));
			m_divModesCrossSection.insert(m_divModesCrossSection.begin() + i * m_newEdgeCtrlPointId + m_ctrlPointCountI, divModeCrossSection(i, m_newEdgeCtrlPointId - 1));
			m_divCommonRatiosCrossSection.insert(m_divCommonRatiosCrossSection.begin() + i * m_newEdgeCtrlPointId + m_ctrlPointCountI, divCommonRatioCrossSection(i, m_newEdgeCtrlPointId - 1));

			addEdgeLinesCrossSectionForSelectionAndPreview(renderer);
		}
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			auto newPolyLineController = new PolyLineController();
			newPolyLineController->setPolyLine(newLines.at(i));
			insertEdgeLineStreamWise(newPolyLineController, i + m_newEdgeCtrlPointId * (m_ctrlPointCountI - 1));
			m_edgeInterpolationStreamWise.insert(m_edgeInterpolationStreamWise.begin() + i + m_newEdgeCtrlPointId * (m_ctrlPointCountI - 1), InterpolationType::Spline);
			m_divModesStreamWise.insert(m_divModesStreamWise.begin() + i + m_newEdgeCtrlPointId * (m_ctrlPointCountI - 1), DivisionMode::Auto);
			m_divCommonRatiosStreamWise.insert(m_divCommonRatiosStreamWise.begin() + i + m_newEdgeCtrlPointId * (m_ctrlPointCountI - 1), 1);

			addEdgeLinesStreamWiseForSelectionAndPreview(renderer);
		}
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			auto pp = subRegionPoissonParameter(i, m_newEdgeCtrlPointId - 1);
			m_subRegionDeployParameters.insert(m_subRegionDeployParameters.begin() + i + m_newEdgeCtrlPointId * (m_ctrlPointCountI - 1), pp);
			addSubRegionPolygon(renderer);
		}

		m_divCountsCrossSection.insert(m_divCountsCrossSection.begin() + m_newEdgeCtrlPointId, 1);
		++m_ctrlPointCountJ;
		updateEdgeLinesForSelection();

		std::vector<PolyLineController*> lines1, lines2;
		for (int i = 0; i < m_ctrlPointCountI; ++i) {
			lines1.push_back(&(edgeLineCrossSectionForEdgeSelection(i, m_newEdgeCtrlPointId - 1)));
			lines2.push_back(&(edgeLineCrossSectionForEdgeSelection(i, m_newEdgeCtrlPointId)));
		}
		double len1 = averageLength(lines1);
		double len2 = averageLength(lines2);
		int newDivCount = m_divCountsCrossSection[m_newEdgeCtrlPointId - 1] + 1;
		int newDivCounts1 = static_cast<int>(newDivCount * len1 / (len1 + len2));
		if (newDivCounts1 == 0) {newDivCounts1 = 1;}
		int newDivCounts2 = newDivCount - newDivCounts1;
		if (newDivCounts2 == 0) {newDivCounts2 = 1;}

		m_divCountsCrossSection[m_newEdgeCtrlPointId - 1] = newDivCounts1;
		m_divCountsCrossSection[m_newEdgeCtrlPointId] = newDivCounts2;
		if (m_newEdgeCtrlPointId <= m_centerLineIndex) {
			++ m_centerLineIndex;
		}
	}

	iRICUndoStack::instance().clear();
	m_regionDefinedMouseEventMode = RegionDefinedMouseEventMode::Normal;
	m_newEdgeLine.clear();

	updateEdgeLinesForSelection();
	m_condition->renderGraphicsView();
}

void GridCreatingConditionLaplace::Impl::updateCenterLineLabelsAndSpline()
{
	std::vector<QPointF> empty;

	auto cline = m_centerLine.polyLine();
	if (cline.size() < 2) {
		m_centerLineSpline.setPolyLine(empty);
	} else {
		m_centerLineSpline.setPolyLine(splineGenerator(m_centerLine.polyLine()));
	}
	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();
	auto col2 = m_condition->actor2DCollection();
	col2->RemoveAllItems();
	setupLabelsForCenterLine(col2);

	m_condition->updateVisibilityWithoutRendering();
}

void GridCreatingConditionLaplace::Impl::updateEdgeLinesForSelection()
{
	// build m_ctrlPointsController

	std::vector<QPointF> points;
	for (int i = 0; i < m_ctrlPointCountI; ++i) {
		for (int j = 0; j < m_ctrlPointCountJ; ++j) {
			points.push_back(ctrlPoint(i, j));
		}
	}
	m_ctrlPointsController.setPolyLine(points);

	// build edgeLineStreamWise
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		std::vector<InterpolationType> itypes;
		std::vector<std::vector<QPointF> > lines;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			itypes.push_back(edgeInterpolationStreamWise(i, j));

			std::vector<QPointF> line;
			line.push_back(ctrlPoint(i, j));
			for (auto p : edgeLineStreamWise(i, j).polyLine()) {
				line.push_back(p);
			}
			line.push_back(ctrlPoint(i + 1, j));
			edgeLineStreamWiseForNodeSelection(i, j).setPolyLine(line);

			lines.push_back(line);
		}
		auto linePoints = buildLinesForSelection(lines, itypes);
		std::vector<std::vector<QPointF> > selLines;

		int offset = 0;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			std::vector<QPointF> line;
			size_t pointCount = (lines.at(i).size() - 1) * SUBDIVNUM + 1;
			for (int k = 0; k < pointCount; ++k) {
				line.push_back(linePoints.at(k + offset));
			}
			edgeLineStreamWiseForEdgeSelection(i, j).setPolyLine(line);
			offset += pointCount - 1;
			selLines.push_back(line);
		}
	}

	// build edgeLineCrossSection
	for (int i = 0; i < m_ctrlPointCountI; ++i) {
		std::vector<InterpolationType> itypes;
		std::vector<std::vector<QPointF> > lines;
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			itypes.push_back(edgeInterpolationCrossSection(i, j));

			std::vector<QPointF> line;
			line.push_back(ctrlPoint(i, j));
			for (auto p : edgeLineCrossSection(i, j).polyLine()) {
				line.push_back(p);
			}
			line.push_back(ctrlPoint(i, j + 1));
			edgeLineCrossSectionForNodeSelection(i, j).setPolyLine(line);

			lines.push_back(line);
		}
		auto linePoints = buildLinesForSelection(lines, itypes);
		std::vector<std::vector<QPointF> > selLines;

		int offset = 0;
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			std::vector<QPointF> line;
			size_t pointCount = (lines.at(j).size() - 1) * SUBDIVNUM + 1;
			for (int k = 0; k < pointCount; ++k) {
				line.push_back(linePoints.at(k + offset));
			}
			edgeLineCrossSectionForEdgeSelection(i, j).setPolyLine(line);
			offset += pointCount - 1;
			selLines.push_back(line);
		}
	}

	// build subRegionPolygons
	for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			QPolygonF polygon;
			polygon.push_back(ctrlPoint(i, j));
			auto line = edgeLineStreamWiseForEdgeSelection(i, j).polyLine();
			line.erase(line.begin());
			line.pop_back();
			for (auto p : line) {
				polygon.push_back(p);
			}
			polygon.push_back(ctrlPoint(i + 1, j));
			line = edgeLineCrossSectionForEdgeSelection(i + 1, j).polyLine();
			line.erase(line.begin());
			line.pop_back();
			for (auto p : line) {
				polygon.push_back(p);
			}
			polygon.push_back(ctrlPoint(i + 1, j + 1));
			line = edgeLineStreamWiseForEdgeSelection(i, j + 1).polyLine();
			std::reverse(line.begin(), line.end());
			line.erase(line.begin());
			line.pop_back();
			for (auto p : line) {
				polygon.push_back(p);
			}
			polygon.push_back(ctrlPoint(i, j + 1));
			line = edgeLineCrossSectionForEdgeSelection(i, j).polyLine();
			std::reverse(line.begin(), line.end());
			line.erase(line.begin());
			line.pop_back();
			for (auto p : line) {
				polygon.push_back(p);
			}
			polygon.push_back(ctrlPoint(i, j));
			subRegionPolygons(i, j).setPolygon(polygon);
		}
	}

	updateEdgeLinesForDivisionPreview();

	setupLabelsForRegionDefined();
	optimizeCommonRatios();
}

void GridCreatingConditionLaplace::Impl::updateEdgeLinesForDivisionPreview()
{
	// streamWise
	for (int j = 0; j < m_ctrlPointCountJ; ++j) {
		std::vector<std::vector<QPointF> > selLines;
		std::vector<DivisionMode> divModes;
		std::vector<double> commonRatios;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			selLines.push_back(edgeLineStreamWiseForEdgeSelection(i, j).polyLine());
			divModes.push_back(divModeStreamWise(i, j));
			commonRatios.push_back(divCommonRatioStreamWise(i, j));
		}
		auto previewPoints = buildLinesForDivisionPreview(selLines, m_divCountsStreamWise, divModes, commonRatios);

		int offset = 0;
		for (int i = 0; i < m_ctrlPointCountI - 1; ++i) {
			std::vector<QPointF> line;
			for (int k = 0; k <= m_divCountsStreamWise[i]; ++k) {
				line.push_back(previewPoints.at(k + offset));
			}
			edgeLineStreamWiseForDivisionPreview(i, j).setPolyLine(line);
			offset += m_divCountsStreamWise[i];
		}
	}

	// crossSection
	for (int i = 0; i < m_ctrlPointCountI; ++i) {
		std::vector<std::vector<QPointF> > selLines;
		std::vector<DivisionMode> divModes;
		std::vector<double> commonRatios;
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			selLines.push_back(edgeLineCrossSectionForEdgeSelection(i, j).polyLine());
			divModes.push_back(divModeCrossSection(i, j));
			commonRatios.push_back(divCommonRatioCrossSection(i, j));
		}
		auto previewPoints = buildLinesForDivisionPreview(selLines, m_divCountsCrossSection, divModes, commonRatios);

		int offset = 0;
		for (int j = 0; j < m_ctrlPointCountJ - 1; ++j) {
			std::vector<QPointF> line;
			for (int k = 0; k <= m_divCountsCrossSection[j]; ++k) {
				line.push_back(previewPoints.at(k + offset));
			}
			edgeLineCrossSectionForDivisionPreview(i, j).setPolyLine(line);
			offset += m_divCountsCrossSection[j];
		}
	}
}

void GridCreatingConditionLaplace::Impl::clearCtrlPointsAndEdges()
{
	m_ctrlPointCountI = 0;
	m_ctrlPointCountJ = 0;
	m_ctrlPoints.clear();

	m_edgeInterpolationStreamWise.clear();
	m_edgeInterpolationCrossSection.clear();
	m_divCountsStreamWise.clear();
	m_divCountsCrossSection.clear();
	m_divModesStreamWise.clear();
	m_divModesCrossSection.clear();
	m_divCommonRatiosStreamWise.clear();
	m_divCommonRatiosCrossSection.clear();

	auto col = m_condition->actorCollection();
	auto r = m_condition->renderer();
	clearPolylines(&m_edgeLinesStreamWise, col, r);
	clearPolylines(&m_edgeLinesCrossSection, col, r);
	clearPolylines(&m_edgeLinesStreamWiseForNodeSelection, col, r);
	clearPolylines(&m_edgeLinesCrossSectionForNodeSelection, col, r);
	clearPolylines(&m_edgeLinesStreamWiseForEdgeSelection, col, r);
	clearPolylines(&m_edgeLinesCrossSectionForEdgeSelection, col, r);
	clearPolylines(&m_edgeLinesStreamWiseForDivisionPreview, col, r);
	clearPolylines(&m_edgeLinesCrossSectionForDivisionPreview, col, r);
	clearPolygons(&m_subRegionPolygons, col, r);
}

void GridCreatingConditionLaplace::Impl::setupLabelsForCenterLine(vtkActor2DCollection* col)
{
	auto line = m_centerLine.polyLine();
	if (line.size() < 2) {return;}

	auto firstp = line.front();
	m_upstreamActor.setPosition(firstp);
	col->AddItem(m_upstreamActor.actor());
	auto lastp = line.back();
	m_downstreamActor.setPosition(lastp);
	col->AddItem(m_downstreamActor.actor());
}

void GridCreatingConditionLaplace::Impl::setupLabelsForRegionDefined()
{
	auto firstp = ctrlPoint(0, m_centerLineIndex);
	m_upstreamActor.setPosition(firstp);

	auto lastp = ctrlPoint(m_ctrlPointCountI - 1, m_centerLineIndex);
	m_downstreamActor.setPosition(lastp);
}

void GridCreatingConditionLaplace::Impl::requestPreviewSubRegionGrid()
{
	if (m_selectedSubRegionId == -1) {return;}

	int i = m_selectedSubRegionId % (m_ctrlPointCountI - 1);
	int j = m_selectedSubRegionId / (m_ctrlPointCountI - 1);

	m_buildThread->restartBuild(i, j);
}

void GridCreatingConditionLaplace::Impl::pushCenterLineStartDefinitionCommand(bool keyDown, const QPoint& p)
{
	pushCenterLineUpdateLabelsAndSplineCommand(new CenterLineStartDefinitionCommand(keyDown, p, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineFinishDefinitionCommand()
{
	iRICUndoStack::instance().undo();
	pushCenterLineUpdateLabelsAndSplineCommand(new CenterLineFinishDefiningCommand(m_condition));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineAddVertexCommand(bool keyDown, const QPoint& pos)
{
	pushCenterLineUpdateLabelsAndSplineCommand(new PolyLineAddVertexCommand(keyDown, &m_centerLine, m_hoveredEdgeId, pos, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to)
{
	pushCenterLineUpdateLabelsAndSplineCommand(new PolyLineMoveVertexCommand(keyDown, from, to, &m_centerLine, m_hoveredVertexId, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineRemoveVertexCommand()
{
	pushCenterLineUpdateLabelsAndSplineCommand(new PolyLineRemoveVertexCommand(m_hoveredVertexId, &m_centerLine));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineEditCoordinatesCommand(std::vector<QPointF>& coords)
{
	pushCenterLineUpdateLabelsAndSplineCommand(new PolyLineEditCoordinatesCommand(coords, &m_centerLine));
}

void GridCreatingConditionLaplace::Impl::pushCenterLineDefineNewVertexCommand(bool keyDown, const QPoint& pos)
{
	pushCenterLineUpdateLabelsAndSplineCommand(new PolyLineDefineNewVertexCommand(keyDown, pos, &m_centerLine, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushEdgeEditCoordinatesCommand(bool streamWise, int i, int j, std::vector<QPointF>& coords, bool apply)
{
	pushUpdateLineForEdgeSelectionCommand(new EdgeEditCoordinatesCommand(streamWise, i, j, coords, this), apply);
}

void GridCreatingConditionLaplace::Impl::pushCenterLineUpdateLabelsAndSplineCommand(QUndoCommand* comm)
{
	m_condition->pushRenderCommand(new CenterLineUpdateLabelsAndSplineCommand(comm, this));
}

void GridCreatingConditionLaplace::Impl::pushNewEdgeStartDefinitionCommand(bool keyDown, const QPoint& pos, PreProcessorGraphicsViewInterface* v)
{
	if (keyDown) {
		double x = pos.x();
		double y = pos.y();
		v->viewportToWorld(x, y);
		QPointF clickedP(x, y);

		QPointF leg;
		bool ok = getPerpendicularLineOfLeg(clickedP, *hoveredSectionForEdgeSelection(), &leg);
		if (! ok) {return;}
		m_condition->pushRenderCommand(new NewEdgeStartDefinitionCommand(leg, m_condition));
	} else {
		m_condition->pushRenderCommand(new NewEdgeStartDefinitionCommand(pos, m_condition));
	}
}

void GridCreatingConditionLaplace::Impl::pushNewEdgeDefineNewVertexCommand(bool keyDown, const QPoint& pos)
{
	m_condition->pushRenderCommand(new PolyLineDefineNewVertexCommand(keyDown, pos, &m_newEdgeLine, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushEdgeMoveCommand(bool keyDown, const QPoint& from, const QPoint& to)
{
	if (m_hoveredSectionEdgeType == EdgeType::StreamWise) {
		auto edge = hoveredSectionForNodeSelection();
		if (m_hoveredVertexId == 0) {
			int j = m_hoveredSectionId / (m_ctrlPointCountI - 1);
			int i = m_hoveredSectionId - j * (m_ctrlPointCountI - 1);
			pushCtrlPointMoveCommand(false, from, to, i + j * m_ctrlPointCountI);
		} else if (m_hoveredVertexId == edge->polyLine().size() - 1) {
			int j = m_hoveredSectionId / (m_ctrlPointCountI - 1);
			int i = m_hoveredSectionId - j * (m_ctrlPointCountI - 1);
			pushCtrlPointMoveCommand(false, from, to, (i + 1) + j * m_ctrlPointCountI);
		} else {
			pushUpdateLineForEdgeSelectionCommand(new PolyLineMoveVertexCommand(keyDown, from, to, hoveredSection(), m_hoveredVertexId - 1, m_condition));
		}
	} else if (m_hoveredSectionEdgeType == EdgeType::CrossSection) {
		auto edge = hoveredSectionForNodeSelection();
		if (m_hoveredVertexId == 0) {
			pushCtrlPointMoveCommand(false, from, to, m_hoveredSectionId);
		} else if (m_hoveredVertexId == edge->polyLine().size() - 1) {
			pushCtrlPointMoveCommand(false, from, to, m_hoveredSectionId + m_ctrlPointCountI);
		} else {
			pushUpdateLineForEdgeSelectionCommand(new PolyLineMoveVertexCommand(keyDown, from, to, hoveredSection(), m_hoveredVertexId - 1, m_condition));
		}
	}
}

void GridCreatingConditionLaplace::Impl::pushEdgeAddVertexCommand(bool keyDown, const QPoint& pos)
{
	pushUpdateLineForEdgeSelectionCommand(new PolyLineAddVertexCommand(keyDown, hoveredSection(), m_hoveredEdgeId - 1, pos, m_condition));
}

void GridCreatingConditionLaplace::Impl::pushEdgeRemoveVertexCommand()
{
	pushUpdateLineForEdgeSelectionCommand(new PolyLineRemoveVertexCommand(m_hoveredVertexId - 1, hoveredSection()));
}

void GridCreatingConditionLaplace::Impl::pushCtrlPointMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, int pointId)
{
	pushUpdateLineForEdgeSelectionCommand(new CtrlPointMoveCommand(keyDown, from, to, pointId,m_condition));
}

void GridCreatingConditionLaplace::Impl::pushEdgeSetInterpolationModeCommand(InterpolationType type)
{
	if (m_selectedSectionEdgeType == EdgeType::None) {return;}

	pushUpdateLineForEdgeSelectionCommand(new EdgeSetInterpolationModeCommand(m_selectedSectionEdgeType, m_selectedSectionId, type, this));
	updateActionStatus();
}

void GridCreatingConditionLaplace::Impl::pushDeploySettingCommand(bool streamWise, int edgeId, DivisionMode mode, double commonRatio)
{
	pushUpdateLineForEdgeSelectionCommand(new DeploySettingCommand(streamWise, edgeId, mode, commonRatio, this));
}

void GridCreatingConditionLaplace::Impl::pushDivisionSettingCommand(bool streamWise, int edgeId, int divNum, DivisionMode mode, double commonRatio, bool thisLineOnly)
{
	pushUpdateLineForEdgeSelectionCommand(new DivisionSettingCommand(streamWise, edgeId, divNum, mode, commonRatio, thisLineOnly, this));
}

void GridCreatingConditionLaplace::Impl::pushWholeRegionDivisionSettingCommand(const std::vector<int>& streamWiseDivCounts, const std::vector<int>& crossSectionDivCounts)
{
	pushUpdateLineForEdgeSelectionCommand(new WholeRegionDivisionSettingCommand(streamWiseDivCounts, crossSectionDivCounts, this));
}

void GridCreatingConditionLaplace::Impl::pushSubRegionDeploySettingCommand(const DeployParameter& param, int index)
{
	pushUpdateLineForEdgeSelectionCommand(new SubRegionDeploySettingCommand(param, index, this));
}

void GridCreatingConditionLaplace::Impl::pushUpdateLineForEdgeSelectionCommand(QUndoCommand* comm, bool renderRedoOnly)
{
	if (renderRedoOnly) {
		m_condition->pushRenderRedoOnlyCommand(new UpdateEdgeLinesForEdgeSelectionCommand(comm, this));
	}	else {
		m_condition->pushRenderCommand(new UpdateEdgeLinesForEdgeSelectionCommand(comm, this));
	}
}

void GridCreatingConditionLaplace::Impl::optimizeCommonRatios()
{
	m_optimizeThread->restartOpt();
}

std::vector<QPointF> GridCreatingConditionLaplace::Impl::buildLinesForSelection(const std::vector<std::vector<QPointF> >& lines, const std::vector<InterpolationType>& types)
{
	std::vector<QPointF> ret;

	int idx = 0;
	std::vector<QPointF> linesForSpline;

	while (idx < types.size()) {
		auto lineToAdd = lines[idx];
		if (types[idx] == InterpolationType::Spline) {
			if (linesForSpline.size() > 0) {
				lineToAdd.erase(lineToAdd.begin());
			}
			for (auto p : lineToAdd) {
				linesForSpline.push_back(p);
			}
		} else {
			if (linesForSpline.size() > 0) {
				addLinePoints(linesForSpline, &ret, &splineGenerator);
				linesForSpline.clear();
			}
			addLinePoints(lineToAdd, &ret, &linearGenerator);
		}
		++ idx;
	}
	if (linesForSpline.size() > 0) {
		addLinePoints(linesForSpline, &ret, &splineGenerator);
	}
	return ret;
}

std::vector<QPointF> GridCreatingConditionLaplace::Impl::buildLinesForDivisionPreview(const std::vector<std::vector<QPointF> >& lines, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios)
{
	std::vector<QPointF> ret;

	// @todo auto is not implemented correctly yet. auto is now equal.
	for (int i = 0; i < lines.size(); ++i) {
		auto line = lines.at(i);
		auto divNum = divNums.at(i);
		auto divMode = divModes.at(i);
		auto commonRatio = commonRatios.at(i);

		auto lengths = buildLengths(line);

		std::vector<double> lengthsForPoints;
		if (divMode == DivisionMode::Auto) {
			// @todo fix this
			lengthsForPoints = buildLengthsForPoints(lengths.back(), divNum, DivisionMode::Geometric, commonRatio);
		} else {
			lengthsForPoints = buildLengthsForPoints(lengths.back(), divNum, divMode, commonRatio);
		}

		for (int i = 0; i < lengthsForPoints.size(); ++i) {
			if (ret.size() > 0 && i == 0) {continue;}

			double l = lengthsForPoints.at(i);
			ret.push_back(getPointForLength(l, lengths, line));
		}
	}

	return ret;
}

std::vector<double> GridCreatingConditionLaplace::Impl::buildLengthsForPoints(double length, int divNum, DivisionMode divMode, double commonRatio)
{
	std::vector<double> ret;
	if (divMode == DivisionMode::Equally) {
		for (int i = 0; i <= divNum; ++i) {
			ret.push_back((length * i) / divNum);
		}
	} else if (divMode == DivisionMode::Geometric) {
		if (commonRatio == 1) {
			for (int i = 0; i <= divNum; ++i) {
				ret.push_back((length * i) / divNum);
			}
		} else {
			auto a = (commonRatio - 1) / (std::pow(commonRatio, divNum) - 1) * length;
			double sum = 0;
			double item = a;
			for (int i = 0; i < divNum; ++i) {
				ret.push_back(sum);
				sum += item;
				item *= commonRatio;
			}
			ret.push_back(length);
		}
	}
	return ret;
}

std::vector<double> GridCreatingConditionLaplace::Impl::buildLengths(const std::vector<QPointF>& line)
{
	std::vector<double> ret;

	ret.push_back(0);
	double lenSum = 0;
	for (int i = 0; i < line.size() - 1; ++i) {
		lenSum += iRIC::length(line.at(i + 1) - line.at(i));
		ret.push_back(lenSum);
	}

	return ret;
}

double GridCreatingConditionLaplace::Impl::edgeOptimizeFunc(const std::vector<double>& lengths, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios)
{
	std::vector<std::pair<double, double> > firstAndLastEdgeLengths;

	double r_weight = 0.5;
	double ret = 0;

	for (int i = 0; i < lengths.size(); ++i) {
		auto len = lengths.at(i);
		auto divNum = divNums.at(i);
		auto divMode = divModes.at(i);
		auto commonRatio = commonRatios.at(i);

		double r = commonRatio;
		if (r < 1) {r = 1 / r;}
		if (divMode == DivisionMode::Auto) {
			ret += r_weight * r;
		}
		std::vector<double> lengthsForPoints;
		if (divMode == DivisionMode::Auto) {
			lengthsForPoints = buildLengthsForPoints(len, divNum, DivisionMode::Geometric, commonRatio);
		} else {
			lengthsForPoints = buildLengthsForPoints(len, divNum, divMode, commonRatio);
		}
		auto first = *(lengthsForPoints.begin() + 1) - *(lengthsForPoints.begin());
		auto last = *(lengthsForPoints.begin() + lengthsForPoints.size() - 1) - *(lengthsForPoints.begin() + lengthsForPoints.size() - 2);
		firstAndLastEdgeLengths.push_back(std::make_pair(first, last));
	}

	for (int i = 0; i < firstAndLastEdgeLengths.size() - 1; ++i) {
		auto& pair1 = firstAndLastEdgeLengths[i];
		auto& pair2 = firstAndLastEdgeLengths[i + 1];

		double ratio = pair1.second / pair2.first;
		if (ratio < 1) {ratio = 1 / ratio;}
		ret += ratio;
	}
	return ret;
}

double GridCreatingConditionLaplace::Impl::edgeOptimizeFuncEx(int n, const double* x0, void* ex)
{
	auto params = reinterpret_cast<CROptimizeParams*> (ex);
	std::vector<double> ratios = params->commonRatios;
	int idx = 0;
	for (int i = 0; i < params->divModes.size(); ++i) {
		if (params->divModes.at(i) == GridCreatingConditionLaplace::DivisionMode::Auto) {
			ratios[i] = *(x0 + idx);
			++ idx;
		}
	}
	return edgeOptimizeFunc(params->lengths, params->divNums, params->divModes, ratios);
}

void GridCreatingConditionLaplace::Impl::makeLineWideWithPoints(PolyLineController* controller)
{
	controller->linesActor()->GetProperty()->SetLineWidth(LINEWIDTH_WIDE);
	controller->pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
}

void GridCreatingConditionLaplace::Impl::makeLineNarrowNoPoints(PolyLineController* controller)
{
	controller->linesActor()->GetProperty()->SetLineWidth(LINEWIDTH_NARROW);
	controller->pointsActor()->GetProperty()->SetPointSize(1);
}

double GridCreatingConditionLaplace::Impl::averageLength(const std::vector<PolyLineController*>& lines)
{
	double tmpLen = 0;
	for (auto line : lines) {
		auto lengths = buildLengths(line->polyLine());
		tmpLen += lengths.back();
	}
	return tmpLen / lines.size();
}

void GridCreatingConditionLaplace::Impl::addEdgeLinesStreamWiseForSelectionAndPreview(vtkRenderer* renderer)
{
	auto newLine2 = new PolyLineController();
	setupEdgeControllerForEdgeSelection(newLine2, m_zDepthRange);
	newLine2->linesActor()->VisibilityOff();
	renderer->AddActor(newLine2->linesActor());
	m_edgeLinesStreamWiseForEdgeSelection.push_back(newLine2);

	auto newLine3 = new PolyLineController();
	m_edgeLinesStreamWiseForNodeSelection.push_back(newLine3);

	auto newLine4 = new PolyLineController();
	setupEdgeControllerForDivisionPreview(newLine4, m_zDepthRange);
	newLine4->pointsActor()->VisibilityOff();
	renderer->AddActor(newLine4->pointsActor());
	m_edgeLinesStreamWiseForDivisionPreview.push_back(newLine4);
}

void GridCreatingConditionLaplace::Impl::addEdgeLinesCrossSectionForSelectionAndPreview(vtkRenderer* renderer)
{
	auto newLine2 = new PolyLineController();
	setupEdgeControllerForEdgeSelection(newLine2, m_zDepthRange);
	newLine2->linesActor()->VisibilityOff();
	renderer->AddActor(newLine2->linesActor());
	m_edgeLinesCrossSectionForEdgeSelection.push_back(newLine2);

	auto newLine3 = new PolyLineController();
	m_edgeLinesCrossSectionForNodeSelection.push_back(newLine3);

	auto newLine4 = new PolyLineController();
	setupEdgeControllerForDivisionPreview(newLine4, m_zDepthRange);
	newLine4->pointsActor()->VisibilityOff();
	renderer->AddActor(newLine4->pointsActor());
	m_edgeLinesCrossSectionForDivisionPreview.push_back(newLine4);
}

void GridCreatingConditionLaplace::Impl::removeEdgeLinesStreamWiseForSelectionAndPreview(vtkActorCollection* col, vtkRenderer* r)
{
	clearPolyline(m_edgeLinesStreamWiseForEdgeSelection.back(), col, r);
	m_edgeLinesStreamWiseForEdgeSelection.pop_back();
	clearPolyline(m_edgeLinesStreamWiseForNodeSelection.back(), col, r);
	m_edgeLinesStreamWiseForNodeSelection.pop_back();
	clearPolyline(m_edgeLinesStreamWiseForDivisionPreview.back(), col, r);
	m_edgeLinesStreamWiseForDivisionPreview.pop_back();
}

void GridCreatingConditionLaplace::Impl::removeEdgeLinesCrossSectionForSelectionAndPreview(vtkActorCollection* col, vtkRenderer* r)
{
	clearPolyline(m_edgeLinesCrossSectionForEdgeSelection.back(), col, r);
	m_edgeLinesCrossSectionForEdgeSelection.pop_back();
	clearPolyline(m_edgeLinesCrossSectionForNodeSelection.back(), col, r);
	m_edgeLinesCrossSectionForNodeSelection.pop_back();
	clearPolyline(m_edgeLinesCrossSectionForDivisionPreview.back(), col, r);
	m_edgeLinesCrossSectionForDivisionPreview.pop_back();
}

void GridCreatingConditionLaplace::Impl::addSubRegionPolygon(vtkRenderer* r)
{
	auto newPolygon = new PolygonController();
	setupSubRegionPolygon(newPolygon, m_zDepthRange);
	newPolygon->paintActor()->VisibilityOff();
	r->AddActor(newPolygon->paintActor());
	m_subRegionPolygons.push_back(newPolygon);
}

void GridCreatingConditionLaplace::Impl::removeSubRegionPolygon(vtkActorCollection* col, vtkRenderer* r)
{
	clearPolygon(m_subRegionPolygons.back(), col, r);
	m_subRegionPolygons.pop_back();
}

void GridCreatingConditionLaplace::Impl::clearDivisionSetting()
{
	DeployParameter defaultPoissonParam;
	for (auto& p : m_subRegionDeployParameters) {
		p = defaultPoissonParam;
	}
	for (auto& c : m_divCountsStreamWise) {
		c = 1;
	}
	for (auto& c : m_divCountsCrossSection) {
		c = 1;
	}
	for (auto& m : m_divModesStreamWise) {
		m = DivisionMode::Auto;
	}
	for (auto& m : m_divModesCrossSection) {
		m = DivisionMode::Auto;
	}
	for (auto& r : m_divCommonRatiosStreamWise) {
		r = 1;
	}
	for (auto& r : m_divCommonRatiosCrossSection) {
		r = 1;
	}
	m_manualDivSetting = false;

	updateEdgeLinesForSelection();
}

void GridCreatingConditionLaplace::Impl::updateManualDivSetting()
{
	m_manualDivSetting = false;
	for (const auto& p : m_subRegionDeployParameters) {
		if (p.setting != DeploySetting::Ratio) {goto SETTRUE;}
		if (p.manualP != 0) {goto SETTRUE;}
		if (p.manualQ != 0) {goto SETTRUE;}
	}
	for (const auto& m : m_divModesStreamWise) {
		if (m != DivisionMode::Auto) {goto SETTRUE;}
	}
	for (const auto& m : m_divModesCrossSection) {
		if (m != DivisionMode::Auto) {goto SETTRUE;}
	}
	for (const auto& r : m_divCommonRatiosStreamWise) {
		if (r != 1) {goto SETTRUE;}
	}
	for (const auto& r : m_divCommonRatiosCrossSection) {
		if (r != 1) {goto SETTRUE;}
	}
	return;

SETTRUE:
	m_manualDivSetting = true;
}
