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

#include <iriclib.h>

class vtkProperty;
class QPolygonF;

class GeoDataRiverSurveyBackgroundGridCreateThread;
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
	vtkSmartPointer<vtkPoints> m_rightBankPoints;

	vtkSmartPointer<vtkUnstructuredGrid> m_rightBankPointSet;
	vtkSmartPointer<vtkStructuredGrid> m_backgroundGrid;

	vtkSmartPointer<vtkUnstructuredGrid> m_blackCrosssection;

	vtkSmartPointer<vtkActor> m_blackCrossectionsActor;

	vtkSmartPointer<vtkActor> m_backgroundActor;

	vtkSmartPointer<vtkStringArray> m_labelArray;
	vtkSmartPointer<vtkLabeledDataMapper> m_labelMapper;
	vtkSmartPointer<vtkActor2D> m_labelActor;

private:
	GeoDataRiverPathPoint* singleSelectedPoint();
	void setupLine(vtkUnstructuredGrid* grid, GeoDataRiverPathPoint* p);

	void allActorsOff();
	void updateSplineSolvers();
	void updateMouseEventMode();
	/// Enable or disable actions depending on the selection status.

	GeoDataRiverPathPoint* m_headPoint;

	GeoDataRiverSurveyBackgroundGridCreateThread* m_gridThread;

	RiverCenterLineSolver m_CenterLineSolver;
	RiverLeftBankSolver m_LeftBankSolver;
	RiverRightBankSolver m_RightBankSolver;

	bool m_definingBoundingBox;
	bool m_leftButtonDown;

	GeoDataRiverSurveyDisplaySetting m_setting;

	QPoint m_dragStartPoint;
	QPoint m_currentPoint;

	Qt::KeyboardModifiers m_keyboardModifiers;
	GridCreatingConditionRiverSurveyInterface* m_gridCreatingCondition;

private:
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
