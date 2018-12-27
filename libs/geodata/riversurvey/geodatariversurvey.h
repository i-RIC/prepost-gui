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
#include <vtkLabeledDataMapper.h>
#include <vtkActor2D.h>
#include <vtkStringArray.h>

#include <QPoint>
#include <QCursor>
#include <QPixmap>

#include <iriclib.h>

class vtkProperty;
class QAction;
class QPolygonF;

class GeoDataRiverSurveyCrosssectionWindow;
class GeoDataRiverPathPointMoveDialog;
class GeoDataRiverPathPointShiftDialog;
class GeoDataRiverPathPointRenameDialog;
class GeoDataRiverPathPointRotateDialog;
class GeoDataRiverPathPointExtensionAddDialog;
class GeoDataRiverPathPointExpandDialog;
class GeoDataRiverPathPointInsertDialog;
class GeoDataRiverSurveyBackgroundGridCreateThread;
class GeoDataRiverCrosssectionAltitudeMoveDialog;
class GeoDataRiverSurveyCrosssectionWindowGraphicsView;
class GridCreatingConditionRiverSurveyInterface;
class GeoDataRiverSurveyProxy;

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

	enum MouseEventMode {
		meNormal,
		meTranslate,
		meTranslatePrepare,
		meRotateRight,
		meRotatePrepareRight,
		meRotateLeft,
		meRotatePrepareLeft,
		meShift,
		meShiftPrepare,
		meMoveExtentionEndPointLeft,
		meMoveExtensionEndPointPrepareLeft,
		meMoveExtentionEndPointRight,
		meMoveExtensionEndPointPrepareRight,
		meExpansionRight,
		meExpansionPrepareRight,
		meExpansionLeft,
		meExpansionPrepareLeft,
		meAddingExtension,
		meInserting,

		meTranslateDialog,
		meRotateDialog,
		meShiftDialog,
		meExpansionDialog
	};
	GeoDataRiverSurvey(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* att);
	~GeoDataRiverSurvey();

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
	QColor doubleToColor(double d);
	void setupScalarArray();
	void updateInterpolators();
	void updateShapeData();
	void updateSelectionShapeData();
	GeoDataRiverPathPoint* headPoint() const;
	vtkStructuredGrid* backgroundGrid() const;
	void updateCrossectionWindows();
	void setColoredPoints(GeoDataRiverPathPoint* black, GeoDataRiverPathPoint* red, GeoDataRiverPathPoint* blue);
	void setGridCreatingCondition(GridCreatingConditionRiverSurveyInterface* cond);
	GridCreatingConditionRiverSurveyInterface* gridCreatingCondition() const;
	void useDivisionPointsForBackgroundGrid(bool use);
	void refreshBackgroundGrid();
	void cancelBackgroundGridUpdate();
	void toggleCrosssectionWindowsGridCreatingMode(bool gridMode);
	void informCtrlPointUpdateToCrosssectionWindows();

	GeoDataProxy* getProxy() override;

private slots:
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

signals:
	void dataUpdated();

protected:
	const static int LINEDIVS = 36;
	void updateMouseCursor(PreProcessorGraphicsViewInterface* v);
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void loadExternalData(const QString& filename) override;
	void saveExternalData(const QString& filename) override;
	void updateFilename() override;
	int iRICLibType() const override;
	void doApplyOffset(double x, double y) override;
	/// The pointdata, that has the positions of
	/// River center, left bank, and right bank
	vtkSmartPointer<vtkPoints> m_points;
	vtkSmartPointer<vtkPoints> m_rightBankPoints;

	vtkSmartPointer<vtkUnstructuredGrid> m_riverCenters;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedRiverCenters;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedLeftBanks;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedRightBanks;
	vtkSmartPointer<vtkUnstructuredGrid> m_rightBankPointSet;

	vtkSmartPointer<vtkStructuredGrid> m_riverCenterLine;
	vtkSmartPointer<vtkStructuredGrid> m_leftBankLine;
	vtkSmartPointer<vtkStructuredGrid> m_rightBankLine;

	vtkSmartPointer<vtkStructuredGrid> m_backgroundGrid;

	vtkSmartPointer<vtkUnstructuredGrid> m_firstAndLastCrosssections;
	vtkSmartPointer<vtkUnstructuredGrid> m_crosssections;
	vtkSmartPointer<vtkUnstructuredGrid> m_selectedCrosssections;
	vtkSmartPointer<vtkUnstructuredGrid> m_crosssectionLines;

	vtkSmartPointer<vtkUnstructuredGrid> m_blackCrosssection;
	vtkSmartPointer<vtkUnstructuredGrid> m_redCrosssection;
	vtkSmartPointer<vtkUnstructuredGrid> m_blueCrosssection;

	vtkSmartPointer<vtkActor> m_riverCenterActor;
	vtkSmartPointer<vtkActor> m_selectedRiverCenterActor;
	vtkSmartPointer<vtkActor> m_selectedLeftBankActor;
	vtkSmartPointer<vtkActor> m_selectedRightBankActor;

	vtkSmartPointer<vtkLODActor> m_riverCenterLineActor;
	vtkSmartPointer<vtkLODActor> m_leftBankLineActor;
	vtkSmartPointer<vtkLODActor> m_rightBankLineActor;

	vtkSmartPointer<vtkActor> m_firstAndLastCrosssectionsActor;
	vtkSmartPointer<vtkActor> m_crossectionsActor;
	vtkSmartPointer<vtkActor> m_selectedCrossectionsActor;

	vtkSmartPointer<vtkActor> m_blackCrossectionsActor;
	vtkSmartPointer<vtkActor> m_redCrossectionsActor;
	vtkSmartPointer<vtkActor> m_blueCrossectionsActor;

	vtkSmartPointer<vtkActor> m_backgroundActor;
	vtkSmartPointer<vtkActor> m_crosssectionLinesActor;

	vtkSmartPointer<vtkStringArray> m_labelArray;
	vtkSmartPointer<vtkLabeledDataMapper> m_labelMapper;
	vtkSmartPointer<vtkActor2D> m_labelActor;

private:
	GeoDataRiverPathPoint* selectedPoint();
	void setupLine(vtkUnstructuredGrid* grid, GeoDataRiverPathPoint* p);

	void allActorsOff();
	void updateSplineSolvers();
	void setupCursors();
	void setupActions();
	void updateMouseEventMode();
	/// Enable or disable actions depending on the selection status.
	void updateActionStatus();

	QPixmap m_pixmapMove;
	QPixmap m_pixmapRotate;
	QPixmap m_pixmapExpand;
	QPixmap m_pixmapShift;

	QCursor m_cursorMove;
	QCursor m_cursorRotate;
	QCursor m_cursorExpand;
	QCursor m_cursorShift;

	GeoDataRiverPathPoint* m_headPoint;

	GeoDataRiverSurveyBackgroundGridCreateThread* m_gridThread;

	RiverCenterLineSolver m_CenterLineSolver;
	RiverLeftBankSolver m_LeftBankSolver;
	RiverRightBankSolver m_RightBankSolver;

	QAction* m_addUpperSideAction;
	QAction* m_addLowerSideAction;
	QAction* m_moveAction;
	QAction* m_rotateAction;
	QAction* m_shiftAction;
	QAction* m_expandAction;
	QAction* m_deleteAction;
	QAction* m_renameAction;
	QAction* m_addLeftExtensionPointAction;
	QAction* m_addRightExtensionPointAction;
	QAction* m_removeLeftExtensionPointAction;
	QAction* m_removeRightExtensionPointAction;
	QAction* m_openCrossSectionWindowAction;
	QAction* m_showBackgroundAction;
	QAction* m_interpolateSplineAction;
	QAction* m_interpolateLinearAction;

	QMenu* m_rightClickingMenu;

	bool m_definingBoundingBox;
	bool m_leftButtonDown;

	GeoDataRiverSurveyDisplaySetting m_setting;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	MouseEventMode m_mouseEventMode;
	Qt::KeyboardModifiers m_keyboardModifiers;
	GridCreatingConditionRiverSurveyInterface* m_gridCreatingCondition;

private:
	class AddExtensionCommand;
	class ChangeSelectionCommand;
	class DeleteRiverPathPointCommand;
	class EditCrosssectionCommand;
	class EditDisplaySettingCommand;
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
	friend class GeoDataRiverSurveyCrosssectionWindow;
	friend class GeoDataRiverSurveyCrosssectionWindowGraphicsView;

	friend class GeoDataRiverSurveyProxy;
};

#endif // GEODATAPOLYGON_H
