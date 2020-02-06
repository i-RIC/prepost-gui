#ifndef GEODATARIVERSURVEY_H
#define GEODATARIVERSURVEY_H

#include "gd_riversurvey_global.h"
#include <guicore/pre/geodata/geodata.h>
#include "geodatariverpathpoint.h"
#include "geodatarivershapeinterpolator.h"
#include "geodatariversurveydisplaysetting.h"

#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkStructuredGrid.h>

#include <QPoint>

#include <iriclib.h>

class vtkPolyData;

class GridCreatingConditionRiverSurveyInterface;

/// Polygon container.
/**
 * GeoDataPolygon uses vtkPolygon instance as the container (m_polygon),
 * but has interfaces to use QPolygon instances for input / output.
 * QPolygon has easier api to define polygons.
 */
class GD_RIVERSURVEY_EXPORT GeoDataRiverSurvey : public GeoData
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
	void informSelection(PreProcessorGraphicsViewInterface* v) override;
	void informDeselection(PreProcessorGraphicsViewInterface* v) override;
	void addCustomMenuItems(QMenu* menu) override;
	void viewOperationEnded(PreProcessorGraphicsViewInterface* v) override;
	void keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void keyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v) override;
	void updateZDepthRangeItemCount(ZDepthRange& range) override;
	void assignActorZValues(const ZDepthRange& range) override;
	bool getValueRange(double* min, double* max) override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* d) override;

	void showInitialDialog() override;

	void updateInterpolators();
	void updateShapeData();
	void updateSelectionShapeData();
	GeoDataRiverPathPoint* headPoint() const;
	vtkStructuredGrid* backgroundGrid() const;
	void updateCrosssectionWindows();
	void setColoredPoints(GeoDataRiverPathPoint* black);
	void setGridCreatingCondition(GridCreatingConditionRiverSurveyInterface* cond);
	GridCreatingConditionRiverSurveyInterface* gridCreatingCondition() const;
	void useDivisionPointsForBackgroundGrid(bool use);

	void refreshBackgroundGrid();
	void cancelBackgroundGridUpdate();

	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode);
	void informCtrlPointUpdateToCrosssectionWindows();

	GeoDataProxy* getProxy() override;

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

signals:
	void dataUpdated();

protected:
	const static int LINEDIVS = 36;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void updateFilename() override;
	int iRICLibType() const override;
	void doApplyOffset(double x, double y) override;

private:
	void createModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void createModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void createModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void createModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void createModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void createModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);

	void editModeKeyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void editModeKeyReleaseEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* v);
	void editModeMouseDoubleClickEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void editModeMouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void editModeMousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);
	void editModeMouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v);

	void pushUpdateLabelsCommand(QUndoCommand* com, bool renderRedoOnly = false);
	void finishDefiningLine();

	GeoDataRiverPathPoint* singleSelectedPoint();
	void setupLine(vtkPolyData* polyData, GeoDataRiverPathPoint* p);

	void allActorsOff();
	void updateSplineSolvers();

	GeoDataRiverPathPoint* m_headPoint;

	RiverCenterLineSolver m_CenterLineSolver;
	RiverLeftBankSolver m_LeftBankSolver;
	RiverRightBankSolver m_RightBankSolver;

	GeoDataRiverSurveyDisplaySetting m_setting;

private:
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
	class SetDisplaySettingCommand;
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

#ifdef _DEBUG
	#include "private/geodatariversurvey_impl.h"
#endif // _DEBUG

#endif // GEODATAPOLYGON_H
