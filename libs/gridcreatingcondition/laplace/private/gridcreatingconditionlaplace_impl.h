#ifndef GRIDCREATINGCONDITIONLAPLACE_IMPL_H
#define GRIDCREATINGCONDITIONLAPLACE_IMPL_H

#include "../gridcreatingconditionlaplace.h"

#include <guibase/polygon/polygoncontroller.h>
#include <guibase/polyline/polylinecontroller.h>
#include <guibase/vtktool/vtklabel2dactor.h>
#include <misc/zdepthrange.h>

#include <QCursor>
#include <QPixmap>
#include <QPointF>

class GeoDataRiverSurvey;
class Structured2DGrid;

class vtkActor;
class vtkDataSetMapper;

class QAction;
class QMenu;

class GridCreatingConditionLaplace::Impl
{
public:
	enum class CenterLineOnlyMouseEventMode {
		Normal,
		BeforeDefining,
		Defining,
		MoveVertex,
		MoveVertexPrepare,
		AddVertex,
		AddVertexPrepare,
		AddVertexNotPossible,
		RemoveVertexPrepare,
		RemoveVertexNotPossible,
		EditCoodinatesDialog,
	};

	enum class RegionDefinedMouseEventMode {
		Normal,
		AddEdgeLinePrepare,
		AddEdgeLineNotPossible,
		AddEdgeLine,
		AddEdgeLineFinishPrepare,
		MoveVertex,
		MoveVertexPrepare,
		AddVertex,
		AddVertexPrepare,
		AddVertexNotPossible,
		RemoveVertexPrepare,
		RemoveVertexNotPossible,
		EditCoodinatesDialog,
	};

	enum class InterpolationType {
		None,
		Linear,
		Spline,
	};

	enum class EdgeType {
		None,
		StreamWise,
		CrossSection
	};

	enum class EditMode {
		CenterLineOnly,
		RegionDefined
	};

	Impl(GridCreatingConditionLaplace* cond);
	~Impl();

	Structured2DGrid* createGrid();
	Structured2DGrid* createSubRegionGrid(int i, int j);
	static std::vector<QPointF> buildSubGrid(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp);
	static std::vector<QPointF> buildSubGridByRatio(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp);
	static std::vector<QPointF> buildSubGridByPoisson(const std::vector<QPointF>& edgeLineStreamWise1, const std::vector<QPointF>& edgeLineStreamWise2, const std::vector<QPointF>& edgeLineCrossSection1, const std::vector<QPointF>& edgeLineCrossSection2, const DeployParameter& pp);

	int ctrlPointCountI() const;
	int ctrlpointCountJ() const;
	QPointF& ctrlPoint(int i, int j);
	bool divideSettingExists() const;

	void copyCenterLine(GeoDataRiverSurvey* data, int num);
	static int getNumPoints(GeoDataRiverSurvey* riverSurvey);

	InterpolationType& edgeInterpolationStreamWise(int i, int j);
	InterpolationType& edgeInterpolationCrossSection(int i, int j);
	PolyLineController& edgeLineStreamWise(int i, int j);
	PolyLineController& edgeLineCrossSection(int i, int j);
	PolyLineController& edgeLineStreamWiseForNodeSelection(int i, int j);
	PolyLineController& edgeLineCrossSectionForNodeSelection(int i, int j);
	PolyLineController& edgeLineStreamWiseForEdgeSelection(int i, int j);
	PolyLineController& edgeLineCrossSectionForEdgeSelection(int i, int j);
	PolyLineController& edgeLineStreamWiseForDivisionPreview(int i, int j);
	PolyLineController& edgeLineCrossSectionForDivisionPreview(int i, int j);
	PolygonController& subRegionPolygons(int i, int j);

	void insertEdgeLineStreamWise(PolyLineController* line, int idx = -1);
	void insertEdgeLineCrossSection(PolyLineController* line, int idx = -1);

	DivisionMode& divModeStreamWise(int i, int j);
	DivisionMode& divModeCrossSection(int i, int j);

	double& divCommonRatioStreamWise(int i, int j);
	double& divCommonRatioCrossSection(int i, int j);

	DeployParameter& subRegionPoissonParameter(int i, int j);

	PolyLineController* selectedSectionForEdgeSelection() const;
	InterpolationType selectedSectionInterpolateType() const;

	PolyLineController* hoveredSection() const;
	PolyLineController* hoveredSectionForNodeSelection() const;
	PolyLineController* hoveredSectionForEdgeSelection() const;

	void removeEdgeLineStreamWise(int idx);
	void removeEdgeLineCrossSection(int idx);

	void updateActorSetting();
	void updateActionStatus();
	void updateMouseEventMode(const QPoint& pos, bool updateSelect = false);
	PolyLineController* updateHoveredSection(const QPoint& pos);
	PolygonController* updateSelectedSubRegion(const QPoint& pos);
	bool setupNewEdgeSetting();
	bool isNewEdgeFinishReady();
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);

	void buildBankLines();
	void addNewEdge(const QPoint& pos, PreProcessorGraphicsViewInterface* v);

	void updateCenterLineLabelsAndSpline();
	void updateEdgeLinesForSelection();
	void updateEdgeLinesForDivisionPreview();
	void clearCtrlPointsAndEdges();

	void setupLabelsForCenterLine(vtkActor2DCollection* col);
	void setupLabelsForRegionDefined();

	void requestPreviewSubRegionGrid();

	void pushCenterLineStartDefinitionCommand(bool keyDown, const QPoint& p);
	void pushCenterLineFinishDefinitionCommand();
	void pushCenterLineAddVertexCommand(bool keyDown, const QPoint& pos);
	void pushCenterLineMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to);
	void pushCenterLineRemoveVertexCommand();
	void pushCenterLineEditCoordinatesCommand(std::vector<QPointF>& coords);
	void pushCenterLineDefineNewVertexCommand(bool keyDown, const QPoint& pos);
	void pushEdgeEditCoordinatesCommand(bool streamWise, int i, int j, std::vector<QPointF>& coords, bool apply = false);
	void pushCenterLineUpdateLabelsAndSplineCommand(QUndoCommand* comm);

	void pushNewEdgeStartDefinitionCommand(bool keyDown, const QPoint& pos, PreProcessorGraphicsViewInterface* v);
	void pushNewEdgeDefineNewVertexCommand(bool keyDown, const QPoint& pos);

	void pushEdgeMoveCommand(bool keyDown, const QPoint& from, const QPoint& to);
	void pushEdgeAddVertexCommand(bool keyDown, const QPoint& pos);
	void pushEdgeRemoveVertexCommand();
	void pushCtrlPointMoveCommand(bool keyDown, const QPoint& from, const QPoint& to, int pointId);
	void pushEdgeSetInterpolationModeCommand(InterpolationType type);
	void pushDeploySettingCommand(bool streamWise, int edgeId, DivisionMode mode, double commonRatio);
	void pushDivisionSettingCommand(bool streamWise, int edgeId, int divNum, DivisionMode mode, double commonRatio, bool thisLineOnly);
	void pushWholeRegionDivisionSettingCommand(const std::vector<int>& streamWiseDivCounts, const std::vector<int>& crossSectionDivCounts);
	void pushSubRegionDeploySettingCommand(const DeployParameter& param, int index);
	void pushUpdateLineForEdgeSelectionCommand(QUndoCommand* comm, bool renderRedoOnly = false);

	void optimizeCommonRatios();

	static std::vector<QPointF> buildLinesForSelection(const std::vector<std::vector<QPointF> >& lines, const std::vector<InterpolationType>& types);
	static std::vector<QPointF> buildLinesForDivisionPreview(const std::vector<std::vector<QPointF> >& lines, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios);
	static std::vector<double> buildLengthsForPoints(double length, int divNum, DivisionMode divMode, double commonRatio);
	static std::vector<double> buildLengths(const std::vector<QPointF>& line);
	static double edgeOptimizeFunc(const std::vector<double>& lengths, const std::vector<int>& divNums, const std::vector<DivisionMode>& divModes, const std::vector<double>& commonRatios);
	static double edgeOptimizeFuncEx(int n, const double* x0, void* ex);

	static void makeLineWideWithPoints(PolyLineController* controller);
	static void makeLineNarrowNoPoints(PolyLineController* controller);
	static double averageLength(const std::vector<PolyLineController*>& lines);

	void addEdgeLinesStreamWiseForSelectionAndPreview(vtkRenderer* renderer);
	void addEdgeLinesCrossSectionForSelectionAndPreview(vtkRenderer* renderer);
	void addSubRegionPolygon(vtkRenderer* r);
	void removeEdgeLinesStreamWiseForSelectionAndPreview(vtkActorCollection* col, vtkRenderer* r);
	void removeEdgeLinesCrossSectionForSelectionAndPreview(vtkActorCollection* col, vtkRenderer* r);
	void removeSubRegionPolygon(vtkActorCollection* col, vtkRenderer* r);
	void clearDivisionSetting();
	void updateManualDivSetting();

	template<typename V>
	static void loadValueVector(QDataStream* stream, std::vector<V>* vals);
	template<typename V>
	static void saveValueVector(QDataStream* stream, const std::vector<V>& vals);

	template<typename V>
	static void loadEnumVector(QDataStream* stream, std::vector<V>* vals);
	template<typename V>
	static void saveEnumVector(QDataStream* stream, const std::vector<V>& vals);

	EditMode m_editMode;
	CenterLineOnlyMouseEventMode m_centerLineOnlyMouseEventMode;
	RegionDefinedMouseEventMode m_regionDefinedMouseEventMode;

	// For CenterLineOnly mode
	PolyLineController m_centerLine;
	PolyLineController m_centerLineSpline;

	int m_centerLineIndex;

	// for RegionDefined mode
	int m_ctrlPointCountI;
	int m_ctrlPointCountJ;

	int m_loadVersion;

	std::vector<QPointF> m_ctrlPoints;
	PolyLineController m_ctrlPointsController;

	std::vector<InterpolationType> m_edgeInterpolationStreamWise;
	std::vector<InterpolationType> m_edgeInterpolationCrossSection;

	std::vector<int> m_divCountsStreamWise;
	std::vector<int> m_divCountsCrossSection;

	std::vector<DivisionMode> m_divModesStreamWise;
	std::vector<DivisionMode> m_divModesCrossSection;

	std::vector<double> m_divCommonRatiosStreamWise;
	std::vector<double> m_divCommonRatiosCrossSection;
	std::vector<double> m_tmpDivCommonRatiosStreamWise;
	std::vector<double> m_tmpDivCommonRatiosCrossSection;

	std::vector<DeployParameter> m_subRegionDeployParameters;

	std::vector<PolyLineController*> m_edgeLinesStreamWise;
	std::vector<PolyLineController*> m_edgeLinesCrossSection;

	std::vector<PolyLineController*> m_edgeLinesStreamWiseForNodeSelection;
	std::vector<PolyLineController*> m_edgeLinesCrossSectionForNodeSelection;

	std::vector<PolyLineController*> m_edgeLinesStreamWiseForEdgeSelection;
	std::vector<PolyLineController*> m_edgeLinesCrossSectionForEdgeSelection;

	std::vector<PolyLineController*> m_edgeLinesStreamWiseForDivisionPreview;
	std::vector<PolyLineController*> m_edgeLinesCrossSectionForDivisionPreview;

	std::vector<PolygonController*> m_subRegionPolygons;
	Structured2DGrid* m_previewGrid;
	std::vector<QPointF> m_previewGridPoints;
	vtkDataSetMapper* m_previewGridMapper;
	vtkActor* m_previewGridActor;

	CommonRatioOptimizeThread* m_optimizeThread;
	BuildSubGridPointsThread* m_buildThread;

	vtkLabel2DActor m_upstreamActor;
	vtkLabel2DActor m_downstreamActor;

	EdgeType m_selectedSectionEdgeType;
	int m_selectedSectionId;

	int m_selectedSubRegionId;

	EdgeType m_hoveredSectionEdgeType;
	int m_hoveredSectionId;
	int m_hoveredEdgeId;
	int m_hoveredVertexId;

	EdgeType m_newEdgeType;
	int m_newEdgeCtrlPointId;
	int m_newEdgeStartEdgeId;
	int m_newEdgeEndEdgeId;
	bool m_newEdgeFromZero;
	PolyLineController m_newEdgeLine;

	bool m_itemSelected;
	ZDepthRange m_zDepthRange;
	bool m_manualDivSetting;

	double m_previousLeftBankDistance;
	double m_previousRightBankDistance;

	QPoint m_previousPos;
	QPoint m_pressPos;

	QPixmap m_addCursorPixmap;
	QPixmap m_removeCursorPixmap;

	QCursor m_addCursor;
	QCursor m_removeCursor;

	QMenu* m_rightClickingMenu;

	QAction* m_buildBankLinesAction;

	QMenu* m_interpolateMenu;
	QAction* m_interpolateSplineAction;
	QAction* m_interpolateLinearAction;

	QAction* m_addNewEdgeAction;
	QAction* m_joinRegionsAction;

	QAction* m_addVertexAction;
	QAction* m_removeVertexAction;
	QAction* m_editCoordinatesAction;

	QAction* m_wholeRegionDivisionSettingAction;
	QAction* m_divisionSettingAction;
	QAction* m_deploySettingAction;
	QAction* m_deploySubRegionSettingAction;
	QAction* m_clearDivisionSettingAction;

private:
	GridCreatingConditionLaplace* m_condition;
};

template<typename V>
void GridCreatingConditionLaplace::Impl::loadValueVector(QDataStream* stream, std::vector<V>* vals)
{
	int size;
	*stream >> size;
	for (int i = 0; i < size; ++i) {
		V v;
		*stream >> v;
		vals->push_back(v);
	}
}

template<typename V>
void GridCreatingConditionLaplace::Impl::saveValueVector(QDataStream* stream, const std::vector<V>& vals)
{
	int size = static_cast<int> (vals.size());
	*stream << size;
	for (V v : vals) {
		*stream << v;
	}
}

template<typename V>
void GridCreatingConditionLaplace::Impl::loadEnumVector(QDataStream* stream, std::vector<V>* vals)
{
	int size;
	*stream >> size;
	for (int i = 0; i < size; ++i) {
		int v;
		*stream >> v;
		vals->push_back(static_cast<V>(v));
	}
}

template<typename V>
void GridCreatingConditionLaplace::Impl::saveEnumVector(QDataStream* stream, const std::vector<V>& vals)
{
	int size = static_cast<int> (vals.size());
	*stream << size;
	for (V v : vals) {
		int intv = static_cast<int>(v);
		*stream << intv;
	}
}

#endif // GRIDCREATINGCONDITIONLAPLACE_IMPL_H
