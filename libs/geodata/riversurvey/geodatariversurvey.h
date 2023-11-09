#ifndef GEODATARIVERSURVEY_H
#define GEODATARIVERSURVEY_H

#include "gd_riversurvey_global.h"
#include <guicore/pre/geodata/geodatawithsinglemapper.h>
#include "geodatariverpathpoint.h"
#include "geodatarivershapeinterpolator.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStructuredGrid.h>

#include <QPoint>

class vtkLabeledDataMapper;
class vtkPolyData;
class vtkStringArray;

class GridCreatingConditionRiverSurveyI;

/// Polygon container.
/**
 * GeoDataPolygon uses vtkPolygon instance as the container (m_polygon),
 * but has interfaces to use QPolygon instances for input / output.
 * QPolygon has easier api to define polygons.
 */
class GD_RIVERSURVEY_EXPORT GeoDataRiverSurvey : public GeoDataWithSingleMapper
{
	Q_OBJECT

public:
	static const int WSE_NAME_MAXLENGTH;

	GeoDataRiverSurvey(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);
	~GeoDataRiverSurvey();

	void setEditMode();

	void setCaption(const QString& cap) override;
	void setupActors() override;
	void setupMenu() override;
	void setupDataItem() override;
	bool addToolBarButtons(QToolBar* parent) override;
	void informSelection(PreProcessorGraphicsViewI* v) override;
	void informDeselection(PreProcessorGraphicsViewI* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewI* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	bool getValueRange(double* min, double* max) override;
	void showPropertyDialog() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void applyColorMapSetting() override;

	void showInitialDialog() override;

	void updateInterpolators();
	void updateShapeData();
	void updateActorSetting() override;
	void updateSelectionShapeData();
	GeoDataRiverPathPoint* headPoint() const;
	vtkStructuredGrid* backgroundGrid() const;
	vtkCell* findBackgroundGridCell(double x, double y, double* weight) const;
	void updateCrosssectionWindows();
	void setColoredPoints(GeoDataRiverPathPoint* black);
	void setGridCreatingCondition(GridCreatingConditionRiverSurveyI* cond);
	GridCreatingConditionRiverSurveyI* gridCreatingCondition() const;
	void useDivisionPointsForBackgroundGrid(bool use);

	void refreshBackgroundGrid();
	void cancelBackgroundGridUpdate();

	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode);
	void informCtrlPointUpdateToCrosssectionWindows();

	GeoDataProxy* getProxy() override;

	class DisplaySettingWidget;

private slots:
	void generateData();
	void buildBankLines();
	void addVertexMode(bool on);
	void removeVertexMode(bool on);
	void importCenterLine();
	void exportCenterLine();

	void moveSelectedPoints();
	void deleteSelectedPoints();
	void shiftSelectedPoints();
	void expandSelectedPoints();
	void rotateSelectedPoint();
	void renameSelectedPoint();
	void addLeftExtensionPoint();
	void addRightExtensionPoint();
	void removeLeftExtensionPoint();
	void removeRightExtensionPoint();
	void restoreMouseEventMode();
	void insertNewPoint();
	void addNewPoint();
	void updateBackgroundGrid();
	void openCrossSectionWindow();
	void displaySetting();
	void switchInterpolateModeToLinear();
	void switchInterpolateModeToSpline();
	void mapPointsData();
	void generatePointMap();

signals:
	void dataUpdated();

private:
	const static int LINEDIVS = 36;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void updateFilename() override;
	iRICLib::H5CgnsGeographicDataGroup::Type iRICLibType() const override;
	void doApplyOffset(double x, double y) override;

	void createModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);
	void createModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);
	void createModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void createModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void createModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void createModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);

	void editModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);
	void editModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewI* v);
	void editModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void editModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void editModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);
	void editModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v);

	void pushUpdateLabelsCommand(QUndoCommand* com, bool renderRedoOnly = false);
	void finishDefiningLine();

	GeoDataRiverPathPoint* singleSelectedPoint();
	void setupLine(vtkPolyData* polyData, GeoDataRiverPathPoint* p);

	void allActorsOff();
	void updateSplineSolvers();

	vtkPolyData* centerAndBankLines() const;
	vtkPolyData* crossSectionLines() const;
	vtkUnstructuredGrid* rightBankPointSet() const;
	vtkStringArray* labelArray() const;

	static void setupLabelMapper(vtkLabeledDataMapper* mapper);

	GeoDataRiverPathPoint* m_headPoint;

	RiverCenterLineSolver m_CenterLineSolver;
	RiverLeftBankSolver m_LeftBankSolver;
	RiverRightBankSolver m_RightBankSolver;

	class DisplaySetting;

	class PolyLineFinishDefiningCommand;
	class PolyLineUpdateLabelsCommand;
	class PolyLineCoordinatesEditor;

	class AddExtensionCommand;
	class ChangeSelectionCommand;
	class DeleteRiverPathPointCommand;
	class EditCrosssectionCommand;
	class ExpandCrosssectionCommand;
	class InsertRiverPathPointCommand;
	class MouseEditCrosssectionCommand;
	class MouseMoveExtensionCommand;
	class MouseRotateRiverCrosssectionCommand;
	class MouseShiftRiverPathCenterCommand;
	class MoveRiverPathPointCommand;
	class RemoveExtensionCommand;
	class RenameRiverPathPointCommand;
	class RotateRiverCrosssectionCommand;
	class ShiftRiverPathCenterCommand;
	class TranslateRiverPathPointCommand;

	class Impl;
	Impl* impl;

public:
	friend class GeoDataRiverSurveyCreator;
	friend class GeoDataRiverSurveyImporter;
	friend class GeoDataRiverPathPointMoveDialog;
	friend class GeoDataRiverPathPointShiftDialog;
	friend class GeoDataRiverPathPointRenameDialog;
	friend class GeoDataRiverPathPointRotateDialog;
	friend class GeoDataRiverPathPointExtensionAddDialog;
	friend class GeoDataRiverPathPointInsertDialog;
	friend class GeoDataRiverPathPointExpandDialog;
	friend class GeoDataRiverCrosssectionAltitudeMoveDialog;
	friend class GeoDataRiverSurveyCrossSectionEditFromPointDialog;
	friend class GeoDataRiverSurveyCrosssectionWindow;
	friend class GeoDataRiverSurveyCrosssectionWindowGraphicsView;

	friend class GeoDataRiverSurveyProxy;
};

#endif // GEODATAPOLYGON_H
